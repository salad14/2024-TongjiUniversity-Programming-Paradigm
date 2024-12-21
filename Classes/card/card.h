#pragma once
#include <string>
#include <functional>
#include "cocos2d.h"
#include "external/json/json.hpp"
using json = nlohmann::json;

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

    CardBase() = default;

    virtual void from_json(const json& j) = 0;

    const std::string& getDescription() const;
    const std::string& getName() const;

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

    // 定义 == 运算符
    bool operator==(const CardBase& other) const {
        return dbfId == other.dbfId &&
            cost == other.cost &&
            cardClass == other.cardClass &&
            name == other.name &&
            text == other.text &&
            type == other.type &&
            rarity == other.rarity;
    }

public:
    int dbfId;
    int cost;
    std::string name;
    std::string text;  // json里面是flavor
    cardClass cardClass;
    cardType type; 
    cardRarity rarity;
};
