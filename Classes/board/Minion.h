#pragma once

#include "cocos2d.h"

class Minion : public cocos2d::Sprite {
public:
    // 构造函数
    Minion(const std::string& name, int attack, int health) : name(name), attack(attack), health(health) {}

    // 获取和设置攻击力
    int getAttack() const { return attack; }
    void setAttack(int attack) { this->attack = attack; }

    // 获取和设置血量
    int getHealth() const { return health; }
    void setHealth(int health) { this->health = health; }

    // 获取名称
    const std::string& getName() const { return name; }

    // 受到攻击
    void takeDamage(int damage) {
        health -= damage;
        if (health < 0) {
            health = 0;
        }
    }

    // 检查是否存活
    bool isAlive() const { return health > 0; }

    // 创建一个随从对象
    static Minion* create(const char* filename) {
        auto sprite = new Minion("Minion", 1, 1); // 默认攻击力和血量为1
        if (sprite && sprite->initWithFile(filename)) {
            sprite->autorelease();
            return sprite;
        }
        delete sprite;
        return nullptr;
    }

private:
    int ID; // 序列
    int attack; // 攻击力
    int health; // 血量
};
