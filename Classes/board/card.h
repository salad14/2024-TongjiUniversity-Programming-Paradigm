#pragma once
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <functional>

#include "cocos2d.h"

class CardBase : public cocos2d::Sprite {
public:
    // ��ȡ����������
    virtual const std::string getDescription() = 0;
    virtual const std::string getName() = 0;

    // ��ȡ�����÷���
    int getCost() const { return cost; }

    virtual void play() = 0;

private:
    std::string ID; // ÿ�ſ�ʹ��һ���ض���ID
    int cost; // ����
    std::string description; // ����
};

enum class mKeyword {
    None = 0,
    Battlecry = 1 << 0,    // ս��
    Deathrattle = 1 << 1,  // ����
    Taunt = 1 << 2,        // ����
    Windfury = 1 << 3,     // ��ŭ
    Charge = 1 << 4,       // ���
    DivineShield = 1 << 5, // ʥ��
    Lifesteal = 1 << 6,    // ��Ѫ
    Frozen = 1 << 7,       // ����
};

// ����ͨ��λ��������ϲ�ͬ�Ĺؼ���
inline mKeyword operator|(mKeyword lhs, mKeyword rhs) {
    return static_cast<mKeyword>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

// ս���Ч�����Ͷ��壺һ���ص�����
using BattlecryEffect = std::function<void()>;

class MinionCard : public CardBase {
public:


private:
    int attack;
    int health;
    int maxhealth;
    mKeyword keyword; // �ؼ���
    BattlecryEffect battlecryEffect; // ս��Ч���ص�

};