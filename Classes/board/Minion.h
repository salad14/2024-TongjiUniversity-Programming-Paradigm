#pragma once

#include <functional>
#include <string>
#include <iostream>
#include "card.h"
#include "cocos2d.h"

class Minion : public cocos2d::Sprite {
public:
    int attack;
    int health;
    int currentHealth;
    std::string name;
    BattlecryEffect battlecryEffect;
    MinionCard* minionCard;  // 存储 MinionCard 的实例

    // 构造函数：通过 MinionCard 初始化 Minion
    Minion(MinionCard* card) {
        minionCard = card;
        attack = card->attack;
        health = card->health;
        currentHealth = health;
        name = card->name;

        // 初始化战吼效果
        battlecryEffect = card->battlecryEffect;

        // 通过 cocos2d::Sprite 初始化精灵（假设有一个图片文件表示随从）
        this->initWithFile("minion_image.png"); // 假设你有一张随从的图片

        // 设置精灵的尺寸等
        this->setContentSize(cocos2d::Size(100, 100));  // 设定大小
    }

    // 触发战吼
    void triggerBattlecry() {
        if (battlecryEffect) {
            std::cout << name << "'s Battlecry triggered!" << std::endl;
            battlecryEffect();  // 调用战吼效果
        }
    }

    // 攻击目标
    void attackTarget(Minion* target) {
        if (target) {
            std::cout << name << " is attacking " << target->name << "!" << std::endl;
            target->takeDamage(attack);
        }
    }

    // 承受伤害
    void takeDamage(int damage) {
        currentHealth -= damage;
        if (currentHealth <= 0) {
            die();
        }
    }

    // 死亡处理
    void die() {
        std::cout << name << " has died!" << std::endl;
        // 处理死亡时的其他逻辑，如触发亡语效果
        if (static_cast<int>(minionCard->keywords) & static_cast<int>(mKeyword::Deathrattle)) {
            std::cout << "Deathrattle triggered!" << std::endl;
            // 亡语效果
        }
        // 在 cocos2d 中移除精灵
        this->removeFromParent();
    }

    // 更新随从的状态或显示
    void update() {
        // 更新随从状态（例如显示剩余生命）
        std::cout << "Minion " << name << " has " << currentHealth << " HP remaining." << std::endl;
    }

    // 其他功能如攻击、嘲讽、风怒等可以在此添加...
};