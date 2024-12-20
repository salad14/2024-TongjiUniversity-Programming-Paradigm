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
        Battlecry = 1 << 0,    // ս��
        Deathrattle = 1 << 1,  // ����
        Taunt = 1 << 2,        // ����
        Windfury = 1 << 3,     // ��ŭ
        Charge = 1 << 4,       // ���
        DivineShield = 1 << 5, // ʥ��
        Lifesteal = 1 << 6,    // ��Ѫ
        Frozen = 1 << 7,       // ����
        Conbo = 1 << 8,        // ����
        Rush = 1 << 9,         // ͻϮ
        // ���� CHOOSE_ONE  // ���� DISCOVER
    };
}

// ����ͨ��λ��������ϲ�ͬ�Ĺؼ���
inline MinionkeyWord::keyWord operator|(MinionkeyWord::keyWord lhs, MinionkeyWord::keyWord rhs) {
    return static_cast<MinionkeyWord::keyWord>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

// ս��������Ч�����Ͷ��壺�ص�����
using BattlecryEffect = std::function<void()>;
using DeathrattleEffect = std::function<void()>;


class MinionCard : public CardBase {
public:

    int attack;
    int health; 
    int maxhealth;
    MinionkeyWord::keyWord keyword;  // �ؼ���
    // ����Ƶ�һЩ���������� ���� ��ÿ������ʱ�� ʹ����  TRIGGER_VISUAL �ؼ���
    // ȫ�ֵĹ��ܸ�ʹ�� AURA �ؼ���

public: // ��������
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
            rarity = cardRarity::COMMON; // Ĭ��Ϊ COMMON
        }
    }

    // �ж��Ƿ����ĳ���ؼ���
    inline bool has_keyWord(MinionkeyWord::keyWord keyWord) const {
        return static_cast<int>(keyword) & static_cast<int>(keyWord);
    }

public:
    // �����ʱ����board����
    void play() override {
        if (has_keyWord(MinionkeyWord::Battlecry)) {
            manager.triggerBattlecry(this->dbfId);
        }
    }

    // ����ʱ����board����
    void die() {
        if (has_keyWord(MinionkeyWord::Deathrattle)) {
            manager.triggerDeathrattle(this->dbfId);
        }
    }
};