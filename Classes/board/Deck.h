#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <random>
#include <ctime>

#include "Card.h"
#include "cocos2d.h"


// Deck �ࣺ��ʾ��ҵ��ƿ�
class Deck : public cocos2d::Sprite{
private:
    std::vector<std::shared_ptr<CardBase>> cards;  // �洢���Ƶ�����

public:
    // ���캯��
    Deck() {}

    // ��ӿ��Ƶ��ƿ�
    void addCard(std::shared_ptr<CardBase> card) {
        cards.push_back(card);
    }

    // ���ƿ��г�һ�ſ���
    std::shared_ptr<CardBase> drawCard() {
        if (cards.empty()) {
            return nullptr;  // ����ƿ�Ϊ�գ����ؿ�ָ��
        }
        std::shared_ptr<CardBase> card = cards.back(); // ��ȡ�ƿ�����һ�ſ���
        cards.pop_back(); // ���ƿ����Ƴ����ſ���
        return card;
    }

    // ϴ�ƣ����ҿ��Ƶ�˳��
    void shuffle() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
    }

    // ��ȡ�ƿ��еĿ�������
    size_t size() const {
        return cards.size();
    }

    // �鿴�ƿ��еĿ��ƣ����ԣ�
    void printDeck() const {
        for (const auto& card : cards) {
            CCLOG("...");
        }
    }

    // ����ƿ�
    void clearDeck() {
        cards.clear();
    }

    // ��������
    virtual ~Deck() {}
};
