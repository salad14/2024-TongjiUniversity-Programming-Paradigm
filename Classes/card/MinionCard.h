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
    // 修改这里，需要Hero.h 一并修改
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

// 可以通过位运算来组合不同的关键词
inline keyWord operator|(keyWord lhs, keyWord rhs) {
    return static_cast<keyWord>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

// 战吼的效果类型定义：一个回调函数
using BattlecryEffect = std::function<void()>;


class MinionCard : public CardBase {
public:

    int attack;
    int health;   // ### 这里 health 是最大生命值，当初始化为前端Sprite时，需要增加 currentHealth
    std::vector<keyWord> mechanics; // 关键词
    // 随从牌的一些触发型条件 比如 “每当抽牌时” 使用了  TRIGGER_VISUAL 关键词
    // 全局的功能更使用 AURA 关键词

public: // 辅助函数
    /*
    仅供测试使用
    MinionCard(int dbfId, std::string name, int cost, int attack, int health)
        : CardBase(dbfId, name, cost), attack(attack), health(health) {}
    */ 

    // 判断是否包含某个关键词
    bool if_keyWord(keyWord keyWord) {
        for (auto i : mechanics)
            if (i == keyWord) return true;
        return false;
    }

public:
    // 打出牌时，由board触发
    void play() override {
        if (if_keyWord(Battlecry)) {
            manager.triggerBattlecry(this->dbfId);
        }
    }

    // 死亡时，由board触发
    void die() {
        if (if_keyWord(Deathrattle)) {
            manager.triggerDeathrattle(this->dbfId);
        }
    }

};