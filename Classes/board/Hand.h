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

	// ��ʾ����
	void displayCard(std::shared_ptr<CardBase> card) {
		// ȷ�� card ��һ���̳��� cocos2d::Sprite �Ķ���
		if (auto spriteCard = dynamic_cast<cocos2d::Sprite*>(card.get())) {
			this->addChild(spriteCard); // ��ӵ� Hand ����ʾ����
			spriteCard->setPosition(this->getPosition()); // ���ÿ���λ��
		}
		else {
			// ��� card ���� cocos2d::Sprite ����
			CCLOG("Error: Card is not a valid cocos2d::Sprite.");
		}
	}

	// �Ƴ����Ƶ���ʾ
	void removeCardDisplay(std::shared_ptr<CardBase> card) {
		// card->removeFromParent();
	}

	// ��ȡ�����е����п���
	const std::vector<std::shared_ptr<CardBase>>& getCards() const {
		return cards;
	}

	// ����������ȡ����
	std::shared_ptr<CardBase> getCardAt(int index) {
		if (index >= 0 && index < cards.size()) {
			return cards[index];
		}
		return nullptr;
	}


	// �������
	void clearHand() {
		cards.clear();
	}

	virtual ~Hand() = default;
};