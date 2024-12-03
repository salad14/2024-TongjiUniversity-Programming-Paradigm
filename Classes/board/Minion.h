#pragma once

#include "cocos2d.h"

class Minion : public cocos2d::Sprite {
public:
    // ���캯��
    Minion(const std::string& name, int attack, int health) : name(name), attack(attack), health(health) {}

    // ��ȡ�����ù�����
    int getAttack() const { return attack; }
    void setAttack(int attack) { this->attack = attack; }

    // ��ȡ������Ѫ��
    int getHealth() const { return health; }
    void setHealth(int health) { this->health = health; }

    // ��ȡ����
    const std::string& getName() const { return name; }

    // �ܵ�����
    void takeDamage(int damage) {
        health -= damage;
        if (health < 0) {
            health = 0;
        }
    }

    // ����Ƿ���
    bool isAlive() const { return health > 0; }

    // ����һ����Ӷ���
    static Minion* create(const char* filename) {
        auto sprite = new Minion("Minion", 1, 1); // Ĭ�Ϲ�������Ѫ��Ϊ1
        if (sprite && sprite->initWithFile(filename)) {
            sprite->autorelease();
            return sprite;
        }
        delete sprite;
        return nullptr;
    }

private:
    int ID; // ����
    int attack; // ������
    int health; // Ѫ��
};
