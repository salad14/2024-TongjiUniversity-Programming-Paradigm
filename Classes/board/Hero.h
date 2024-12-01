#pragma once

#include "cocos2d.h"

enum class HeroType {
    // ��ʦ
    Mage,
    // սʿ
    Warrior,
    // ��ʦ
    Priest,
    // Ǳ����
    Rogue,
    // ����
    Shaman,
};

class Hero : public cocos2d::Sprite {
public:
    // ���캯��
    Hero(const HeroType career, int maxHealth) : career(career), maxHealth(maxHealth), currentHealth(maxHealth) {}

    // ��ȡ������Ѫ��
    int getCurrentHealth() const { return currentHealth; }
    void setCurrentHealth(int health) { currentHealth = std::min(health, maxHealth); }

    int getDemage(int value);

    // ��ȡְҵ
    HeroType getCareer() const { return career; }
private:
    HeroType career; // ְҵ����
    int maxHealth = 30; // ���Ѫ��
    int currentHealth; // ��ǰѪ��
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