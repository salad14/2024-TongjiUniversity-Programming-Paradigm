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
    int health;   // ### 这里 health 是最大生命值，当初始化为前端Sprite时，需要增加 currentHealth
    MinionkeyWord::keyWord keyword;  // 关键词
    // 随从牌的一些触发型条件 比如 “每当抽牌时” 使用了  TRIGGER_VISUAL 关键词
    // 全局的功能更使用 AURA 关键词

public: // 辅助函数
    /*
    仅供测试使用
    MinionCard(int dbfId, std::string name, int cost, int attack, int health)
        : CardBase(dbfId, name, cost), attack(attack), health(health) {}
    */ 
    MinionCard() = default;

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