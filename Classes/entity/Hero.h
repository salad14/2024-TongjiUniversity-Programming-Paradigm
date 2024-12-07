#pragma once

#include "cocos2d.h"

enum class HeroType {
// ����޸�ְҵ����Ҫcard.h cardClassһ���޸�
    // ��ʦ
    MAGE,
    // սʿ
    WARRIOR,
    // Ǳ����
    ROGUE,
    // ��³��
    DRUID,
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
    HeroType career;    // ְҵ����
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