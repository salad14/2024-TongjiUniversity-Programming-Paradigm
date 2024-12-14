// classes/card/SpellCard.h
#pragma once

#include "card/Card.h"
#include "entity/Spell.h"
#include <string>
#include <vector>
#include "cocos2d.h"

namespace Spell {
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
    // 其他法术类别
};

// 前向声明，以避免包含 Manager 和 Minion 的头文件
class Manager;
class Minion;

struct Effect {
    Spell::KeyWord type;
    int amount; // 对于 Damage, Draw, Grow, Crystal 等效果的数值
    // 根据需要添加更多字段，例如目标类型等
};

class SpellCard : public CardBase {
private:
    SpellSchool spellSchool;
    Spell::KeyWord mechanics;
    std::vector<Effect> effects;

public:
    SpellCard(int dbfId, const std::string& name, int cost, cardClass cardClassType,
        const std::string& text, cardRarity rarity, SpellSchool spellSchool)
        : CardBase(dbfId, cost, cardClassType, name, text, cardType::SPELL, rarity),
        spellSchool(spellSchool), mechanics(static_cast<Spell::KeyWord>(0)) {}

    void addMechanic(Spell::KeyWord key) {
        mechanics |= key;
    }

    bool hasMechanic(Spell::KeyWord mechanic) const {
        return Spell::hasKeyword(mechanics, mechanic);
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
};
