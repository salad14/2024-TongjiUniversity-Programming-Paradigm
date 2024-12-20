#pragma once
#include <sstream>
#include <fstream>
#include <map>
#include "card.h"
#include "mechanicsManager/Manager.h"

#include "cocos2d.h"

namespace MinionkeyWord {
    enum keyWord {
        None = 0,
        Battlecry = 1 << 0,    // 战吼
        Deathrattle = 1 << 1,  // 亡语
        Taunt = 1 << 2,        // 嘲讽
        Windfury = 1 << 3,     // 风怒
        Charge = 1 << 4,       // 冲锋
        DivineShield = 1 << 5, // 圣盾
        Lifesteal = 1 << 6,    // 吸血
        Frozen = 1 << 7,       // 冰冻
        Conbo = 1 << 8,        // 连击
        Rush = 1 << 9,         // 突袭
        // 抉择 CHOOSE_ONE  // 发现 DISCOVER
    };
}

// 可以通过位运算来组合不同的关键词
inline MinionkeyWord::keyWord operator|(MinionkeyWord::keyWord lhs, MinionkeyWord::keyWord rhs) {
    return static_cast<MinionkeyWord::keyWord>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

// 战吼和亡语的效果类型定义：回调函数
using BattlecryEffect = std::function<void()>;
using DeathrattleEffect = std::function<void()>;


class MinionCard : public CardBase {
public:

    int attack;
    int health; 
    int maxhealth;
    MinionkeyWord::keyWord keyword;  // 关键词
    // 随从牌的一些触发型条件 比如 “每当抽牌时” 使用了  TRIGGER_VISUAL 关键词
    // 全局的功能更使用 AURA 关键词

public: // 辅助函数
    MinionCard() = default;

    std::map<std::string, ::cardClass> cardClassMap = {
        {"MAGE", cardClass::MAGE},
        {"WARRIOR", cardClass::WARRIOR},
        {"DRUID", cardClass::DRUID},
        {"ROGUE", cardClass::ROGUE},
        {"NEUTRAL", cardClass::NEUTRAL}
    };

    std::map<std::string, ::cardType> cardTypeMap = {
        {"SPELL", cardType::SPELL},
        {"MINION", cardType::MINION},
        {"WEAPON", cardType::WEAPON},
        {"HERO", cardType::HERO}
    };

    std::map<std::string, ::cardRarity> cardRarityMap = {
        {"FREE", cardRarity::COMMON},
        {"COMMON", cardRarity::COMMON},
        {"RARE", cardRarity::COMMON},
        {"EPIC", cardRarity::COMMON},
        {"LEGENDARY", cardRarity::LEGENDARY}
    };

    void from_json(const json& j) override {
        attack = j["attack"].get<int>();
        health = j.at("health").get<int>();
        maxhealth = j.at("health").get<int>();

        // cardBase
        dbfId = j.at("dbfId").get<int>();
        cost = j.at("cost").get<int>();
        name = j.at("name").get<std::string>();
        text = j.at("flavor").get<std::string>();

        // for enum
        std::string cardClassStr = j.at("cardClass").get<std::string>();
        cardClass = cardClassMap.at(cardClassStr);
        std::string typeStr = j.at("type").get<std::string>();
        type = cardTypeMap.at(typeStr);
        std::string cardRarityStr = j.at("rarity").get<std::string>();
        auto it = cardRarityMap.find(cardRarityStr);
        if (it != cardRarityMap.end()) {
            rarity = it->second;
        } else {
            rarity = cardRarity::COMMON; // 默认为 COMMON
        }
    }

    // 判断是否包含某个关键词
    inline bool has_keyWord(MinionkeyWord::keyWord keyWord) const {
        return static_cast<int>(keyword) & static_cast<int>(keyWord);
    }

public:
    // 打出牌时，由board触发
    void play() override {
        if (has_keyWord(MinionkeyWord::Battlecry)) {
            manager.triggerBattlecry(this->dbfId);
        }
    }

    // 死亡时，由board触发
    void die() {
        if (has_keyWord(MinionkeyWord::Deathrattle)) {
            manager.triggerDeathrattle(this->dbfId);
        }
    }
};