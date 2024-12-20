#pragma once

#include "cocos2d.h"
#include "../card/HeroCard.h"

class Hero : public cocos2d::Sprite {
public:
    // 构造函数
    Hero(const HerokeyWord::HeroType career, int maxHealth) : career(career), maxHealth(maxHealth), currentHealth(maxHealth) {}

    // 获取和设置血量
    int getCurrentHealth() const { return currentHealth; }
    void setCurrentHealth(int health) { currentHealth = std::min(health, maxHealth); }

    int getDemage(int value);

    // 获取职业
    HerokeyWord::HeroType getCareer() const { return career; }
private:
    HerokeyWord::HeroType career;    // 职业名称
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