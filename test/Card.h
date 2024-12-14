// test/Card.h
#pragma once

#include <string>

enum class cardClass {
    NEUTRAL,
    MAGE,
    WARRIOR,
    DRUID,
    ROGUE,
    // 其他职业
};

enum class cardType {
    SPELL,
    MINION,
    // 其他卡牌类型
};

enum class cardRarity {
    COMMON,
    RARE,
    EPIC,
    LEGENDARY,
    // 其他稀有度
};

class CardBase {
public:
    int dbfId;
    int cost;
    cardClass classType;
    std::string name;
    std::string text;
    cardType type;
    cardRarity rarity;

    CardBase(int dbfId, int cost, cardClass classType, const std::string& name,
             const std::string& text, cardType type, cardRarity rarity)
        : dbfId(dbfId), cost(cost), classType(classType), name(name),
          text(text), type(type), rarity(rarity) {}

    virtual ~CardBase() = default;

    virtual const std::string& getDescription() const = 0;
    virtual const std::string& getName() const = 0;
};
