#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include "buffer.h"
#include "../base64/base64.h"

// ����DECKSTRING_VERSION �� FormatType ��س���
#define DECKSTRING_VERSION 1

enum FormatType {
    FT_WILD = 1,
    FT_STANDARD = 2,
    //FT_CLASSIC = 3,
    //FT_TWIST = 4
};

using DeckCard = std::pair<int, int>; // [dbfId, count]
using DeckList = std::vector<DeckCard>; // ��Ƭ�б�

// ���ƵĽṹ��
struct DeckDefinition {
    std::vector<DeckCard> cards;
    std::vector<int> heroes; // ʵ����ֻ����һ��Ӣ��
    int format;
};

// �򻯰�ʵ�� ɾ��sideboard (������˹�Ĳ����)
// using SideboardCard = std::tuple<int, int, int>; // [dbfId, count, sideboardOwnerDbfId] 

// ������֤dbf id�Ƿ�Ϊ������
void verifyDbfId(int id, const std::string& name = "dbf id") { // ����Ҫ������
    if (id <= 0) {
        throw std::invalid_argument("Invalid " + name + " " + std::to_string(id) + " (expected valid dbf id)");
    }
}

// ����Ƭ��Ĭ�ϰ�dbfId����sideboardΪtrueʱ��sideboardOwnerDbfId����
template <typename T>
std::vector<T> sort_cards(const std::vector<T>& cards) {
    std::vector<T> sorted_cards = cards;
    std::sort(sorted_cards.begin(), sorted_cards.end(), [](const T& a, const T& b) {
        return std::get<0>(a) < std::get<0>(b);
        });
    return sorted_cards;
}

// �����������࿨Ƭ
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

// ���뺯��
std::string encode(const std::vector<DeckCard>& cards, const std::vector<int>& heroes, int format) {
    BufferWriter writer;

    // ��������Ƿ�Ϸ�
    if (format != FT_WILD && format != FT_STANDARD) {
        throw std::invalid_argument("Invalid deck format");
    }

    // �������
    writer.null();
    writer.varint(DECKSTRING_VERSION);  // �汾��
    writer.varint(format);              // ��ʽ
    writer.varint(heroes.size());       // Ӣ������

    for (int hero : heroes) {
        verifyDbfId(hero, "hero");
        writer.varint(hero);
    }

    // ���򲢱��뿨Ƭ
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

    // ������Ȼ ���Ե��� sideboard �����û�мӱ��� 0
    // writer.varint(0);

    return writer.toString();
}

// ���뺯��
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

    // ���뿨Ƭ // ����Ҳ��Ҫ�޸�
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

    // ���븱��
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

    // ����0 ����ɾ������sideboard
    return { cards, heroes, format };
}