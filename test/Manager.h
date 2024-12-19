// test/Manager.h
#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>
#include "Minion.h"


class Manager {
public:
    // 冻结目标
    void frozen(const std::vector<int>& targetList) {
        for (int targetId : targetList) {
            auto it = minions.find(targetId);
            if (it != minions.end()) {
                it->second->setFrozen(true);
            }
        }
    }

    // 对目标造成伤害
    void damage(const std::vector<int>& targetList, int damage) {
        for (int targetId : targetList) {
            auto it = minions.find(targetId);
            if (it != minions.end()) {
                it->second->takeDamage(damage);
            }
        }
    }

    // 抽卡
    void draw(int num) {
        std::cout << "Drawing " << num << " card(s)." << std::endl;
    }

    // 增强随从
    void grow(Minion& target, int health, int attack, int crystal) {
        target.currentHealth += health;
        target.attack += attack;
        std::cout << "Minion " << target.name << " grows by +" << health << " health and +" << attack << " attack." << std::endl;
    }

    // 增加水晶
    void crystal(int round, int crystal) {
        if (round == -1) {
            // 处理过载
            std::cout << "Handling overload with " << crystal << " crystals." << std::endl;
        }
        else if (round == 0) {
            // 处理本回合临时增加水晶
            std::cout << "Adding " << crystal << " temporary crystals for this round." << std::endl;
        }
        else if (round == 1) {
            // 处理永久增加水晶
            std::cout << "Adding " << crystal << " permanent crystals." << std::endl;
        }
    }

    // 添加随从到管理器
    void addMinion(int id, std::shared_ptr<Minion> minion) {
        minions[id] = minion;
    }

private:
    std::unordered_map<int, std::shared_ptr<Minion>> minions;

    void removeMinion(int id) {
        std::cout << "Minion " << id << " has been removed from the game." << std::endl;
        minions.erase(id);
    }
};
