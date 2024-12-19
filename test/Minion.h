// test/Minion.h
#pragma once

#include "MinionCard.h"
#include <string>
#include <iostream>

class Minion {
public:
    int attack;
    int health;
    int currentHealth;
    std::string name;
    BattlecryEffect battlecryEffect;
    MinionCard* minionCard;  // 存储 MinionCard 的实例
    bool frozen;

    // 构造函数：通过 MinionCard 初始化 Minion
    Minion(MinionCard* card) 
        : minionCard(card), attack(card->attack), health(card->health),
          currentHealth(card->health), name(card->name), 
          battlecryEffect(card->battlecryEffect), frozen(false) {}

    // 触发战吼
    void triggerBattlecry() {
        if (battlecryEffect.effect) {
            std::cout << name << "'s Battlecry triggered!" << std::endl;
            battlecryEffect.effect();  // 调用战吼效果
        }
    }

    // 攻击目标
    void attackTarget(Minion* target) {
        if (target) {
            std::cout << name << " is attacking " << target->name << "!" << std::endl;
            target->takeDamage(attack);
            // 反击伤害（简单示例）
            target->attackTarget(this);
        }
    }

    // 承受伤害
    void takeDamage(int damage) {
        currentHealth -= damage;
        std::cout << name << " takes " << damage << " damage. Current health: " << currentHealth << std::endl;
        if (currentHealth <= 0) {
            die();
        }
    }

    // 死亡处理
    void die() {
        std::cout << name << " has died!" << std::endl;
        // 处理死亡时的其他逻辑，如触发亡语效果
        if (static_cast<int>(minionCard->keywords) & static_cast<int>(keyWord::Deathrattle)) {
            std::cout << "Deathrattle triggered!" << std::endl;
            // 亡语效果
        }
    }

    // 更新随从的状态或显示
    void updateStatus() const {
        std::cout << "Minion " << name << " has " << currentHealth << " HP remaining." << std::endl;
    }

    // 设置冻结状态
    void setFrozen(bool state) {
        frozen = state;
        if (frozen) {
            std::cout << name << " is frozen." << std::endl;
        } else {
            std::cout << name << " is unfrozen." << std::endl;
        }
    }

    bool isFrozen() const {
        return frozen;
    }
};
