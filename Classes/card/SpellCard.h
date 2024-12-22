// classes/card/SpellCard.h
#pragma once

#include <map>
#include "card.h"
#include "../mechanicsManager/Spelleffects.h"
#include "../entity/Minion.h"
#include <vector>
#include "cocos2d.h"

namespace SpellMechanics {
    enum class KeyWord {
        Damage = 1 << 0,
        Draw = 1 << 1,
        Frozen = 1 << 2,
    };

    inline KeyWord operator|(KeyWord lhs, KeyWord rhs) {
        return static_cast<KeyWord>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    inline KeyWord& operator|=(KeyWord& lhs, KeyWord rhs) {
        lhs = lhs | rhs;
        return lhs;
    }

    inline bool hasKeyword(KeyWord mechanics, KeyWord mechanic) {
        return (static_cast<int>(mechanics) & static_cast<int>(mechanic)) != 0;
    }
}

enum class SpellSchool {
    FIRE,
    FROST,
    ARCANE,
};

struct Effect {
    SpellMechanics::KeyWord type;
    int amount; 
};

class SpellCard : public CardBase {
private:
    SpellSchool spellSchool;
    SpellMechanics::KeyWord mechanics;
    std::vector<Effect> effects;

public:
    SpellCard() = default;

    std::map<std::string, ::cardClass> cardClassMap = {
    {"MAGE", cardClass::MAGE},
    {"WARRIOR", cardClass::WARRIOR},
    {"DRUID", cardClass::DRUID},
    {"ROGUE", cardClass::ROGUE},
    {"NEUTRAL", cardClass::NEUTRAL}
    };

    std::map<std::string, ::cardType> cardTypeMap = {
        {"SPELL", cardType::SPELL},
        {"MINION", cardType::MINION},
        {"WEAPON", cardType::WEAPON},
        {"HERO", cardType::HERO}
    };

    std::map<std::string, ::cardRarity> cardRarityMap = {
        {"FREE", cardRarity::COMMON},
        {"COMMON", cardRarity::COMMON},
        {"RARE", cardRarity::COMMON},
        {"EPIC", cardRarity::COMMON},
        {"LEGENDARY", cardRarity::LEGENDARY}
    };

    std::map<std::string, ::SpellSchool> cardSpellMap = {
        {"FIRE", SpellSchool::FIRE},
        {"FROST", SpellSchool::FROST},
        {"ARCANE", SpellSchool::ARCANE},
    };


    SpellCard(int dbfId, const std::string& name, int cost, ::cardClass cardClassType,
        const std::string& text, cardRarity rarity, SpellSchool spellSchool)
        : CardBase(dbfId, cost, cardClassType, name, text, cardType::SPELL, rarity),
        spellSchool(spellSchool), mechanics(static_cast<SpellMechanics::KeyWord>(0)) {}

    void addMechanic(SpellMechanics::KeyWord key) {
        mechanics |= key;
    }

    bool hasMechanic(SpellMechanics::KeyWord mechanic) const {
        return SpellMechanics::hasKeyword(mechanics, mechanic);
    }

    void addEffect(const Effect& effect) {
        effects.push_back(effect);
    }

    const std::vector<Effect>& getEffects() const {
        return effects;
    }

    int getDbfId() const { return dbfId; }
    std::string getName() const { return name; }
    std::string getText() const { return text; }
    int getCost() const { return cost; }
    SpellSchool getSpellSchool() const { return spellSchool; }


    void play() override {}

    /*
    void playSpell(SpellEffects* effects, Minion* minion) override {
        for (const auto& effect : this->getEffects()) {
            switch (effect.type) {
                case SpellMechanics::KeyWord::Damage:
                    effects->damage(minion, effect.amount);
                    break;
                case SpellMechanics::KeyWord::Draw:
                    effects->draw(effect.amount);
                    break;
                case SpellMechanics::KeyWord::Frozen:
                    effects->frozen(minion);
                    break;
                default:
                    break;
            }
        }
    } */

public:
    void from_json(const json& j) override {

        // cardBase
        dbfId = j.at("dbfId").get<int>();
        name = j.at("name").get<std::string>();
        cost = j.at("cost").get<int>();
        text = j.at("text").get<std::string>();

        // for enum
        std::string cardClassStr = j.at("cardClass").get<std::string>();
        cardClass = cardClassMap.at(cardClassStr);
        std::string typeStr = j.at("type").get<std::string>();
        type = cardTypeMap.at(typeStr);
        std::string cardRarityStr = j.at("rarity").get<std::string>();
        auto it = cardRarityMap.find(cardRarityStr);
        if (it != cardRarityMap.end()) {
            rarity = it->second;
        }
        else {
            rarity = cardRarity::COMMON; // ???? COMMON
        }
    }
};
