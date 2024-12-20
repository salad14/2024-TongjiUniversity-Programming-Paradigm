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
    int health;   // ### ���� health ���������ֵ������ʼ��Ϊǰ��Spriteʱ����Ҫ���� currentHealth
    MinionkeyWord::keyWord keyword;  // �ؼ���
    // ����Ƶ�һЩ���������� ���� ��ÿ������ʱ�� ʹ����  TRIGGER_VISUAL �ؼ���
    // ȫ�ֵĹ��ܸ�ʹ�� AURA �ؼ���

public: // ��������
    /*
    ��������ʹ��
    MinionCard(int dbfId, std::string name, int cost, int attack, int health)
        : CardBase(dbfId, name, cost), attack(attack), health(health) {}
    */ 
    MinionCard() = default;

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