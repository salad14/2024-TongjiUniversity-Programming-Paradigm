#pragma once

#include <string>
#include <vector>
#include "MinionCard.h"
//
//#include "cocos2d.h"
//
//
//
//enum class keyWord {
//	Damage = 1 << 0,
//	Draw = 1 << 1,
//	Secret = 1 << 2,
//	Frozen = 1 << 3,
//	Grow = 1 << 4,
//	Crystal = 1 << 5,
//};
//
//inline keyWord operator|(keyWord lhs, keyWord rhs) {
//	return static_cast<keyWord>(static_cast<int>(lhs) | static_cast<int>(rhs));
//}
//
//inline keyWord& operator|=(keyWord& lhs, keyWord rhs) {
//    lhs = lhs | rhs;
//    return lhs;
//}
//
//inline bool hasKeyword(keyWord mechanics, keyWord mechanic) {
//    return (static_cast<int>(mechanics) & static_cast<int>(mechanic)) != 0;
//}
//
//class SpellCard : public CardBase {
//private:
//    SpellSchool spellSchool;
//    keyWord mechanics;
//
//public:
//    SpellCard(int dbfId, const std::string& name, int cost, cardClass cardClass,
//        const std::string& text, cardRarity rarity, SpellSchool spellSchool)
//        : CardBase(dbfId, cost, cardClass, name, text, cardType::SPELL, rarity),
//        spellSchool(spellSchool),
//        mechanics(static_cast<keyWord>(0)) {}
//
//    void addMechanic(keyWord key) {
//        mechanics |= key;
//    }
//
//    bool hasMechanic(keyWord mechanic) const {
//        return hasKeyword(mechanics, mechanic);
//    }
//
//    void play();
//
//    void play(Manager* manager, const std::vector<int>& targetIds, MinionCard* targetMinion = nullptr) {
//        if (hasMechanic(keyWord::Damage)) {
//            manager->damage(targetIds, 6);  // 根据具体卡牌进行参数化
//        }
//        if (hasMechanic(keyWord::Draw)) {
//            manager->draw(1);
//        }
//        if (hasMechanic(keyWord::Frozen)) {
//            manager->frozen(targetIds);
//        }
//        if (hasMechanic(keyWord::Grow) && targetMinion != nullptr) {
//            manager->grow(*targetMinion, 5, 3, 0);
//        }
//        if (hasMechanic(keyWord::Crystal)) {
//            manager->crystal(0, 1);
//        }
//        // 处理其他关键字
//    }
//
//    const std::string& getDescription() const override {
//        return text;
//    }
//
//    const std::string& getName() const override {
//        return name;
//    }
//};
//
