#pragma once
#include <sstream>
#include <fstream>
#include <string>
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

enum mState {
    Taunt,  // 嘲讽
    Charge, // 冲锋
    Stealth,// 潜行
    Frozen,// 冰冻
    Windfury,// 风怒
    lifesteal,// 吸血
    divineshield// 圣盾
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