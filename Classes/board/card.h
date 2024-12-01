#pragma once
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include "cocos2d.h"

class Card : public cocos2d::Sprite {
public:
    // 获取和设置描述
    virtual const std::string getDescription() = 0;
    virtual const std::string getName() = 0;

    // 获取和设置费用
    int getCost() const { return cost; }

    virtual void play() = 0;

private:
    std::string ID;
    int cost; // 费用
};

enum class mKeyword {
    None = 0,
    Battlecry = 1 << 0,    // 战吼
    Deathrattle = 1 << 1,  // 亡语
    Taunt = 1 << 2,        // 嘲讽
    Windfury = 1 << 3,     // 风怒
    Charge = 1 << 4,       // 冲锋
    DivineShield = 1 << 5, // 圣盾
    Lifesteal = 1 << 6,    // 吸血
    Frozen = 1 << 7,       // 冰冻
};

// 可以通过位运算来组合不同的关键词
inline mKeyword operator|(mKeyword lhs, mKeyword rhs) {
    return static_cast<mKeyword>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

class MinionCard : public Card {
public:


private:
    int attack;
    int health;
    int maxhealth;
    mKeyword keyword; // 关键词


};