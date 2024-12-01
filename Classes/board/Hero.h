#pragma once

#include "cocos2d.h"

enum class HeroType {
    // 法师
    Mage,
    // 战士
    Warrior,
    // 牧师
    Priest,
    // 潜行者
    Rogue,
    // 萨满
    Shaman,
};

class Hero : public cocos2d::Sprite {
public:
    // 构造函数
    Hero(const HeroType career, int maxHealth) : career(career), maxHealth(maxHealth), currentHealth(maxHealth) {}

    // 获取和设置血量
    int getCurrentHealth() const { return currentHealth; }
    void setCurrentHealth(int health) { currentHealth = std::min(health, maxHealth); }

    int getDemage(int value);

    // 获取职业
    HeroType getCareer() const { return career; }
private:
    HeroType career; // 职业名称
    int maxHealth = 30; // 最大血量
    int currentHealth; // 当前血量
};

class Mage : public Hero {
};

class Warrior : public Hero {
};

class Priest : public Hero {
};

class Rogue : public Hero {
};

class Shaman : public Hero {
};