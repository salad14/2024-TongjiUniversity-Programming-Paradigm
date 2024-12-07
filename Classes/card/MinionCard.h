#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <functional>
#include "entity/Hero.h"
#include "machanicsManager/Manager.h"

#include "cocos2d.h"

enum cardClass {
    // �޸������ҪHero.h һ���޸�
    MAGE = static_cast<int>(HeroType::MAGE),
    WARRIOR = static_cast<int>(HeroType::WARRIOR),
    ROGUE = static_cast<int>(HeroType::ROGUE),
    DRUID = static_cast<int>(HeroType::DRUID),
    NEUTRAL,
};

enum cardType {
    SPELL,
    MINION,
    WEAPON
};

enum cardRarity {
    COMMAN,
    LEGENDARY,
};

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

// ����ͨ��λ��������ϲ�ͬ�Ĺؼ���
inline keyWord operator|(keyWord lhs, keyWord rhs) {
    return static_cast<keyWord>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

// ս���Ч�����Ͷ��壺һ���ص�����
using BattlecryEffect = std::function<void()>;


class MinionCard : public CardBase {
public:

    int attack;
    int health;   // ### ���� health ���������ֵ������ʼ��Ϊǰ��Spriteʱ����Ҫ���� currentHealth
    std::vector<keyWord> mechanics; // �ؼ���
    // ����Ƶ�һЩ���������� ���� ��ÿ������ʱ�� ʹ����  TRIGGER_VISUAL �ؼ���
    // ȫ�ֵĹ��ܸ�ʹ�� AURA �ؼ���

public: // ��������
    /*
    ��������ʹ��
    MinionCard(int dbfId, std::string name, int cost, int attack, int health)
        : CardBase(dbfId, name, cost), attack(attack), health(health) {}
    */ 

    // �ж��Ƿ����ĳ���ؼ���
    bool if_keyWord(keyWord keyWord) {
        for (auto i : mechanics)
            if (i == keyWord) return true;
        return false;
    }

public:
    // �����ʱ����board����
    void play() override {
        if (if_keyWord(Battlecry)) {
            manager.triggerBattlecry(this->dbfId);
        }
    }

    // ����ʱ����board����
    void die() {
        if (if_keyWord(Deathrattle)) {
            manager.triggerDeathrattle(this->dbfId);
        }
    }

};