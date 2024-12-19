// classes/card/SpellCard.h
#pragma once

#include "card/Card.h"
#include <vector>
#include "cocos2d.h"

namespace SpellMechanics {
    enum class KeyWord {
        Damage = 1 << 0,
        Draw = 1 << 1,
        Frozen = 1 << 2,
        Grow = 1 << 3,
        Crystal = 1 << 4,
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
    // �����������
};

struct Effect {
    SpellMechanics::KeyWord type;
    int amount; // ���� Damage, Draw, Grow, Crystal ��Ч������ֵ
};

class SpellCard : public CardBase {
private:
    SpellSchool spellSchool;
    SpellMechanics::KeyWord mechanics;
    std::vector<Effect> effects;

public:
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

    const std::string& getDescription() const override {
        return text;
    }

    const std::string& getName() const override {
        return name;
    }

    SpellSchool getSpellSchool() const { return spellSchool; }

    void play() override {}
};
