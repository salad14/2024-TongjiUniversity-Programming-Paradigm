#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <random>
#include <ctime>

#include "Card.h"
#include "cocos2d.h"


// Deck 类：表示玩家的牌库
class Deck : public cocos2d::Sprite{
private:
    std::vector<std::shared_ptr<CardBase>> cards;  // 存储卡牌的容器

public:
    // 构造函数
    Deck() {}

    // 添加卡牌到牌库
    void addCard(std::shared_ptr<CardBase> card) {
        cards.push_back(card);
    }

    // 从牌库中抽一张卡牌
    std::shared_ptr<CardBase> drawCard() {
        if (cards.empty()) {
            return nullptr;  // 如果牌库为空，返回空指针
        }
        std::shared_ptr<CardBase> card = cards.back(); // 抽取牌库的最后一张卡牌
        cards.pop_back(); // 从牌库中移除这张卡牌
        return card;
    }

    // 洗牌：打乱卡牌的顺序
    void shuffle() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
    }

    // 获取牌库中的卡牌数量
    size_t size() const {
        return cards.size();
    }

    // 查看牌库中的卡牌（调试）
    void printDeck() const {
        for (const auto& card : cards) {
            CCLOG("...");
        }
    }

    // 清空牌库
    void clearDeck() {
        cards.clear();
    }

    // 析构函数
    virtual ~Deck() {}
};
