#pragma once

#include "cocos2d.h"
#include "../card/HeroCard.h"

class Hero : public cocos2d::Sprite {
public:
    // ���캯��
    Hero(const HerokeyWord::HeroType career, int maxHealth) : career(career), maxHealth(maxHealth), currentHealth(maxHealth) {}

    // ��ȡ������Ѫ��
    int getCurrentHealth() const { return currentHealth; }
    void setCurrentHealth(int health) { currentHealth = std::min(health, maxHealth); }

    int getDemage(int value);

    // ��ȡְҵ
    HerokeyWord::HeroType getCareer() const { return career; }
private:
    HerokeyWord::HeroType career;    // ְҵ����
    int maxHealth = 30; // ���Ѫ��
    int currentHealth;  // ��ǰѪ��
    int armor;          // ����
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