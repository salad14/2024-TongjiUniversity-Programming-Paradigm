#pragma once
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <functional>
#include "Hero.h"
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

class CardBase {
public:
    virtual void play() = 0;

public:
    int dbfId; // 每张卡使用一个特定的ID
    int cost;    // 费用
    cardClass cardClass; // 职业

    std::string name;  // 名字
    std::string text;  // 描述
    cardType type;     // 卡牌类型
    cardRarity rarity; // 稀有度
    keyWord referencedTags; // 关联的关键词  作为前端提示
    std::string id;      // 卡牌ID  形式为 AT_032  和dbfId不同，适用于前端图片URL
    // std::string flavor;  // 卡牌趣闻

public:
    // 仅供测试使用
    CardBase(int dbfId, std::string name, int cost)
        : dbfId(dbfId), name(name), cost(cost) {}

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
    // 仅供测试使用
    MinionCard(int dbfId, std::string name, int cost, int attack, int health)
        : CardBase(dbfId, name, cost), attack(attack), health(health) {}

    // 判断是否包含某个关键词
    bool if_keyWord(keyWord keyWord) {
        for (auto i : mechanics)
            if (i == keyWord) return true;
        return false;
    }

public:
    // 打出牌时，由board触发
    void play() override {
        std::cout << "MinionCard play " << this->name << std::endl;
        if (if_keyWord(Battlecry)) {
            manager.triggerBattlecry(this->dbfId);
        }
    }

    // 死亡时，由board触发
    void die() {
        std::cout << "MinionCard die " << this->name << std::endl;
        if (if_keyWord(Deathrattle)) {
            manager.triggerDeathrattle(this->dbfId);
        }
    }

};
// 法术牌也有mechanics
// 包括奥秘  法术伤害

class SpellCard : public CardBase {
public:
    // Spellschoool // 法术类型 冰霜  火焰  奥数

public:

};