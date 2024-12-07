#pragma once

#include "card/card.h"
#include <vector>
#include <algorithm>
#include "cocos2d.h"

class Hand : public cocos2d::Sprite {
public:
	std::vector<std::shared_ptr<CardBase>> cards;

	Hand() = default;

	void addCard(std::shared_ptr<CardBase> card) {
		cards.push_back(card);
		// displayCard(card);
	}

	void removeCard(std::shared_ptr<CardBase> card) {
		auto it = std::find(cards.begin(), cards.end(), card);
		if (it != cards.end()) {
			cards.erase(it);
		}
		removeCardDisplay(card);
	}

	// 显示卡牌
	void displayCard(std::shared_ptr<CardBase> card) {
		// 确保 card 是一个继承自 cocos2d::Sprite 的对象
		if (auto spriteCard = dynamic_cast<cocos2d::Sprite*>(card.get())) {
			this->addChild(spriteCard); // 添加到 Hand 的显示树中
			spriteCard->setPosition(this->getPosition()); // 设置卡牌位置
		}
		else {
			// 如果 card 不是 cocos2d::Sprite 类型
			CCLOG("Error: Card is not a valid cocos2d::Sprite.");
		}
	}

	// 移除卡牌的显示
	void removeCardDisplay(std::shared_ptr<CardBase> card) {
		// card->removeFromParent();
	}

	// 获取手牌中的所有卡牌
	const std::vector<std::shared_ptr<CardBase>>& getCards() const {
		return cards;
	}

	// 根据索引获取卡牌
	std::shared_ptr<CardBase> getCardAt(int index) {
		if (index >= 0 && index < cards.size()) {
			return cards[index];
		}
		return nullptr;
	}


	// 清空手牌
	void clearHand() {
		cards.clear();
	}

	virtual ~Hand() = default;
};