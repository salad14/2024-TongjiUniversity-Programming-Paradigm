// test/MinionCard.h
#pragma once

#include "Card.h"
#include <functional>

enum class keyWord {
    Deathrattle = 1 << 0,
    Taunt = 1 << 1,
    Windfury = 1 << 2,
    // 其他关键字
};

struct BattlecryEffect {
    std::function<void()> effect;
    // 可以根据需要添加更多字段
};

class MinionCard : public CardBase {
public:
    int attack;
    int health;
    keyWord keywords;
    BattlecryEffect battlecryEffect;

    MinionCard(int dbfId, const std::string& name, int attack, int health,
               keyWord keywords = static_cast<keyWord>(0))
        : CardBase(dbfId, 0, cardClass::NEUTRAL, name, "", cardType::MINION, cardRarity::COMMON),
          attack(attack), health(health), keywords(keywords), currentHealth(health) {}

    int currentHealth;

    // 实现纯虚函数
    const std::string& getDescription() const override {
        return text;
    }

    const std::string& getName() const override {
        return name;
    }
};
