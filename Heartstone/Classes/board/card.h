#pragma once

#include "cocos2d.h"

class Card : public cocos2d::Sprite {
public:
    // 构造函数
    Card(const std::string& name, const std::string& description, int cost) : name(name), description(description), cost(cost) {}

    // 获取和设置名称
    const std::string& getName() const { return name; }

    // 获取和设置描述
    std::string getDescription() const { return description; }

    // 获取和设置费用
    int getCost() const { return cost; }

    // 使用卡牌
    virtual void use() = 0; // 抽象方法，子类必须实现

    // 创建一个卡牌对象
    static Card* create(const char* filename) {
        auto sprite = new Card("Card", "This is a card", 0); // 默认名称、描述和费用
        if (sprite && sprite->initWithFile(filename)) {
            sprite->autorelease();
            return sprite;
        }
        delete sprite;
        return nullptr;
    }

private:
    std::string name; // 名称
    std::string description; // 描述
    int cost; // 费用
};