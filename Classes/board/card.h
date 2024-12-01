#pragma once
#include <sstream>
#include <fstream>
#include <string>
#include "cocos2d.h"

class Card : public cocos2d::Sprite {
public:
    // ��ȡ����������
    virtual const std::string getDescription() = 0;
    virtual const std::string getName() = 0;

    // ��ȡ�����÷���
    int getCost() const { return cost; }

    virtual void play() = 0;

private:
    std::string ID;
    int cost; // ����
};

enum mState {
    Taunt,  // ����
    Charge, // ���
    Stealth,// Ǳ��
    Frozen,// ����
    Windfury,// ��ŭ
    lifesteal,// ��Ѫ
    divineshield// ʥ��
};

class MinionCard : public Card {
public:
    int attack;
    int health;
    int maxhealth;
    bool Taunt;   
    bool Charge;  
    bool Stealth; 
    bool Frozen; 
    bool Windfury; 
    bool lifesteal; 
    bool divineshield; 
};