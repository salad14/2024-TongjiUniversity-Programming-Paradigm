// test/Spell.h
#pragma once

#include <memory>
#include <vector>
#include "SpellCard.h"
#include "Manager.h"
#include "Minion.h"

class Spell {
public:
    Spell(std::shared_ptr<SpellCard> card)
        : card(card), currentCost(card->cost) {}

    // 获取名称
    const std::string& getName() const { return card->name; }

    // 获取描述
    const std::string& getDescription() const { return card->text; }

    // 获取和设置费用
    int getCost() const { return currentCost; }
    void setCost(int newCost) { currentCost = newCost; }

    // 执行法术效果
    void play(Manager* manager, const std::vector<int>& targetIds, Minion* targetMinion = nullptr) {
        for (const auto& effect : card->getEffects()) {
            switch (effect.type) {
                case SpellMechanics::KeyWord::Damage:
                    manager->damage(targetIds, effect.amount);
                    break;
                case SpellMechanics::KeyWord::Draw:
                    manager->draw(effect.amount);
                    break;
                case SpellMechanics::KeyWord::Frozen:
                    manager->frozen(targetIds);
                    break;
                case SpellMechanics::KeyWord::Grow:
                    if (targetMinion != nullptr) {
                        manager->grow(*targetMinion, effect.amount, effect.amount, 0); // 示例
                    }
                    break;
                case SpellMechanics::KeyWord::Crystal:
                    manager->crystal(0, effect.amount);
                    break;
                // 其他效果类型可扩展
                default:
                    break;
            }
        }
    }

private:
    std::shared_ptr<SpellCard> card; // 指向 SpellCard 的共享指针
    int currentCost; // 动态费用
};
