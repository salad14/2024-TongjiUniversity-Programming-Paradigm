#pragma once

#include "card.h"
#include "machanicsManager/SpellManager.h"
#include <string>
#include <vector>
#include "../external/json/json.hpp"

#include "cocos2d.h"

namespace Spell {
    enum class keyWord {
        Damage = 1 << 0,
        Draw = 1 << 1,
        Secret = 1 << 2,
        Frozen = 1 << 3,
        Grow = 1 << 4,
        Crystal = 1 << 5,
    };
}

enum class SpellSchool {
    FIRE,
    FROST,
    ARCANE,
    // 其他法术类别
};

inline Spell::keyWord operator|(Spell::keyWord lhs, Spell::keyWord rhs) {
	return static_cast<Spell::keyWord>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

inline Spell::keyWord& operator|=(Spell::keyWord& lhs, Spell::keyWord rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline bool hasKeyword(Spell::keyWord mechanics, Spell::keyWord mechanic) {
    return (static_cast<int>(mechanics) & static_cast<int>(mechanic)) != 0;
}

class SpellCard : public CardBase {
private:
    SpellSchool spellSchool;
    Spell::keyWord mechanics;

public:
    SpellCard(int dbfId, const std::string& name, int cost, cardClass cardClassType,
        const std::string& text, cardRarity rarity, SpellSchool spellSchool)
        : CardBase(dbfId, cost, cardClassType, name, text, cardType::SPELL, rarity),
        spellSchool(spellSchool),
        mechanics(static_cast<Spell::keyWord>(0)) {}

    void addMechanic(Spell::keyWord key) {
        mechanics |= key;
    }

    bool hasMechanic(Spell::keyWord mechanic) const {
        return hasKeyword(mechanics, mechanic);
    }

    void play() override;

    void play(Manager* manager, const std::vector<int>& targetIds, MinionCard* targetMinion = nullptr) {
        if (hasMechanic(Spell::keyWord::Damage)) {
            manager->damage(targetIds, 6);
        }
        if (hasMechanic(Spell::keyWord::Draw)) {
            manager->draw(1);
        }
        if (hasMechanic(Spell::keyWord::Frozen)) {
            manager->frozen(targetIds);
        }
        if (hasMechanic(Spell::keyWord::Grow) && targetMinion != nullptr) {
            manager->grow(*targetMinion, 5, 3, 0);
        }
        if (hasMechanic(Spell::keyWord::Crystal)) {
            manager->crystal(0, 1);
        }
    }

    const std::string& getDescription() const override {
        return text;
    }

    const std::string& getName() const override {
        return name;
    }
};

