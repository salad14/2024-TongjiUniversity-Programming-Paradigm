#pragma once

#include "cocos2d.h"

enum class HeroType {
// 如果修改职业，需要card.h cardClass一并修改
    // 法师
    MAGE,
    // 战士
    WARRIOR,
    // 潜行者
    ROGUE,
    // 德鲁伊
    DRUID,
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
    HeroType career;    // 职业名称
    int maxHealth = 30; // 最大血量
    int currentHealth;  // 当前血量
    int armor;          // 护甲
};

class MAGE : public Hero {
};

class WARRIOR : public Hero {
};

class ROGUE : public Hero {
};

class Shaman : public Hero {
};

class DRUID : public Hero {
};