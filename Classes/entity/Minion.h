// Minion.h
#pragma once

#include <string>
#include <iostream>
#include "../card/MinionCard.h"

class Minion {
public:
    int attack;
    int health;
    int currentHealth;
    std::string name;
    MinionkeyWord::keyWord keywords;
    bool frozen;
    bool taunt;
    bool windfury;

    // 构造函数：通过 MinionCard 初始化 Minion
    Minion(int dbfId, const std::string& name, int attack, int health, MinionkeyWord::keyWord keywords = MinionkeyWord::None)
        : attack(attack), health(health), currentHealth(health), name(name), keywords(keywords),
        frozen(false), taunt(false), windfury(false) {
        // 根据 keywords 初始化属性
        if (static_cast<int>(keywords) & static_cast<int>(MinionkeyWord::Taunt)) {
            taunt = true;
        }
        if (static_cast<int>(keywords) & static_cast<int>(MinionkeyWord::Windfury)) {
            windfury = true;
        }
    }

    // 触发战吼
    void triggerBattlecry() {
        //std::cout << name << "'s Battlecry triggered!" << std::endl;
    }

    // 攻击目标
    void attackTarget(Minion* target) {
        if (target) {
            //std::cout << name << " is attacking " << target->name << "!" << std::endl;
            target->takeDamage(attack);
        }
    }

    // 承受伤害
    void takeDamage(int damage) {
        currentHealth -= damage;
        //std::cout << name << " takes " << damage << " damage. Current health: " << currentHealth << std::endl;
        if (currentHealth <= 0) {
            die();
        }
    }

    // 死亡处理
    void die() {
        //std::cout << name << " has died!" << std::endl;
        // 亡语效果由 keyWordManager 处理
    }

    // 设置冻结状态
    void setFrozen(bool state) {
        frozen = state;
        if (frozen) {
            //std::cout << name << " is frozen." << std::endl;
        }
        else {
            //std::cout << name << " is unfrozen." << std::endl;
        }
    }

    bool isFrozen() const {
        return frozen;
    }

    // 更新随从的状态或显示
    void updateStatus() const {
        //std::cout << "Minion " << name << " has " << currentHealth << " HP remaining." << std::endl;
    }

    // 增强攻击力和生命值
    void grow(int healthIncrease, int attackIncrease) {
        currentHealth += healthIncrease;
        health += healthIncrease;
        attack += attackIncrease;
        //std::cout << name << " grows by +" << attackIncrease << " attack and +" << healthIncrease << " health." << std::endl;
    }
};
