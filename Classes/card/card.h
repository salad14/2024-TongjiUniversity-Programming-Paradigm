#pragma once
#include <string>
#include <functional>
#include "cocos2d.h"

namespace HerokeyWord {
    enum class HeroType {
        MAGE,
        WARRIOR,
        ROGUE,
        DRUID,
    };
}

enum cardClass {
    MAGE = static_cast<int>(HerokeyWord::HeroType::MAGE),
    WARRIOR = static_cast<int>(HerokeyWord::HeroType::WARRIOR),
    DRUID = static_cast<int>(HerokeyWord::HeroType::DRUID),
    ROGUE = static_cast<int>(HerokeyWord::HeroType::ROGUE),
    NEUTRAL,
};

enum cardType {
    SPELL,
    MINION,
    WEAPON,
    HERO
};

enum cardRarity {
    COMMON,
    LEGENDARY,
};

class CardBase {
public:
    CardBase(int dbfId, int cost, cardClass cardclass, const std::string& name,
        const std::string& text, cardType type, cardRarity rarity)
        : dbfId(dbfId),
        cost(cost),
        cardClass(cardclass),
        name(name),
        text(text),
        type(type),
        rarity(rarity) {}

    virtual const std::string& getDescription() const = 0;
    virtual const std::string& getName() const = 0;

    int getCost() const { return cost; }
    void setCost(const int vary) {
        if (cost + vary >= 0) {
            cost = cost + vary;
        }
        else {
            cost = 0;
        }
    }

    virtual void play() = 0;

public:
    int dbfId; 
    int cost;
    cardClass cardClass;
    
    std::string name;
    std::string text;
    cardType type; 
    cardRarity rarity;
};
