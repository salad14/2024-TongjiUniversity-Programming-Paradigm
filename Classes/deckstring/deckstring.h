#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include "buffer.h"
#include "../base64/base64.h"

// 假设DECKSTRING_VERSION 和 FormatType 相关常量
#define DECKSTRING_VERSION 1

enum FormatType {
    FT_WILD = 1,
    FT_STANDARD = 2,
    //FT_CLASSIC = 3,
    //FT_TWIST = 4
};

using DeckCard = std::pair<int, int>; // [dbfId, count]
using DeckList = std::vector<DeckCard>; // 卡片列表

// 卡牌的结构体
struct DeckDefinition {
    std::vector<DeckCard> cards;
    std::vector<int> heroes; // 实际上只能有一个英雄
    int format;
};

// 简化版实现 删除sideboard (奇利亚斯的侧边栏)
// using SideboardCard = std::tuple<int, int, int>; // [dbfId, count, sideboardOwnerDbfId] 

// 用于验证dbf id是否为正整数
void verifyDbfId(int id, const std::string& name = "dbf id") { // 还需要再完善
    if (id <= 0) {
        throw std::invalid_argument("Invalid " + name + " " + std::to_string(id) + " (expected valid dbf id)");
    }
}

// 排序卡片，默认按dbfId升序，sideboard为true时按sideboardOwnerDbfId升序
template <typename T>
std::vector<T> sort_cards(const std::vector<T>& cards) {
    std::vector<T> sorted_cards = cards;
    std::sort(sorted_cards.begin(), sorted_cards.end(), [](const T& a, const T& b) {
        return std::get<0>(a) < std::get<0>(b);
        });
    return sorted_cards;
}

// 根据数量分类卡片
std::vector<DeckList> trisort_cards(const DeckList& cards) {
    DeckList single = {}, double_cards = {};
    for (const auto& tuple : cards) {
        int card = std::get<0>(tuple);
        int count = std::get<1>(tuple);
        if (count == 1) {
            single.push_back(tuple);
        }
        else if (count == 2) {
            double_cards.push_back(tuple);
        }
        else {
            throw std::invalid_argument("Invalid count " + std::to_string(count) + " (expected positive number)");
        }
    }
    return { single, double_cards };
}

// 编码函数
std::string encode(const std::vector<DeckCard>& cards, const std::vector<int>& heroes, int format) {
    BufferWriter writer;

    // 检查输入是否合法
    if (format != FT_WILD && format != FT_STANDARD) {
        throw std::invalid_argument("Invalid deck format");
    }

    // 编码过程
    writer.null();
    writer.varint(DECKSTRING_VERSION);  // 版本号
    writer.varint(format);              // 格式
    writer.varint(heroes.size());       // 英雄数量

    for (int hero : heroes) {
        verifyDbfId(hero, "hero");
        writer.varint(hero);
    }

    // 排序并编码卡片
    auto sorted_cards = sort_cards(cards);
    for (const auto& list : trisort_cards(sorted_cards)) {
        writer.varint(list.size());
        for (const auto& tuple : list) {
            int card = std::get<0>(tuple);
            int count = std::get<1>(tuple);
            verifyDbfId(card);
            writer.varint(card);
            //if (count != 1 && count != 2) {
            //    writer.varint(count);
            //}
        }
    }

    // 这里虽然 忽略掉了 sideboard ，最后没有加编码 0
    // writer.varint(0);

    return writer.toString();
}

// 解码函数
DeckDefinition decode(const std::string& deckstring) {
    BufferReader reader(deckstring);

    if (reader.nextByte() != 0) {
        throw std::invalid_argument("Invalid deckstring");
    }

    int version = reader.nextByte();
    if (version != DECKSTRING_VERSION) {
        throw std::invalid_argument("Unsupported deckstring version");
    }

    int format = reader.nextVarint();
    if (format != FT_WILD && format != FT_STANDARD) {
        throw std::invalid_argument("Unsupported format in deckstring");
    }

    int heroes_count = reader.nextVarint();
    std::vector<int> heroes;
    for (int i = 0; i < heroes_count; ++i) {
        heroes.push_back(reader.nextVarint());
    }

    // 解码卡片 // 这里也需要修改
    std::vector<DeckCard> cards;
    //for (int i = 0; i < 3; ++i) {
    for (int i = 0; i < 2; ++i) {
        int card_count = reader.nextVarint();
        for (int j = 0; j < card_count; ++j) {
            int card = reader.nextVarint();
            int count = i;
            cards.push_back({ card, count + 1 });
        }
    }

    // 解码副卡
    //std::vector<SideboardCard> sideboardCards;
    //if (reader.nextByte() == 1) {
    //    for (int i = 0; i < 3; ++i) {
    //        int sideboard_count = reader.nextVarint();
    //        for (int j = 0; j < sideboard_count; ++j) {
    //            int card = reader.nextVarint();
    //            int count = (i == 1 || i == 2) ? i : reader.nextVarint();
    //            int owner = reader.nextVarint();
    //            sideboardCards.push_back({ card, count, owner });
    //        }
    //    }
    //}

    // 最后的0 代表删除掉的sideboard
    return { cards, heroes, format };
}