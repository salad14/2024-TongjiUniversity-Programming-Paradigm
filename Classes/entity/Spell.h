// classes/entity/Spell.h
#pragma once

#include <memory>
#include <vector>
#include "../card/SpellCard.h"

class Manager;
class Minion;

class Spell {
public:
    Spell(std::shared_ptr<SpellCard> card)
        : card(card), currentCost(card->getCost()) {}

    // 获取名称
    const std::string& getName() const { return card->getName(); }

    // 获取描述
    const std::string& getDescription() const { return card->getDescription(); }

    // 获取和设置费用
    int getCost() const { return currentCost; }
    void setCost(int newCost) { currentCost = newCost; }

    // 执行法术效果
    void play(Manager* manager, const std::vector<int>& targetIds, Minion* targetMinion = nullptr) {
        for (const auto& effect : card->getEffects()) {
            switch (effect.type) {
                case Spell::KeyWord::Damage:
                    manager->damage(targetIds, effect.amount);
                    break;
                case Spell::KeyWord::Draw:
                    manager->draw(effect.amount);
                    break;
                case Spell::KeyWord::Frozen:
                    manager->frozen(targetIds);
                    break;
                case Spell::KeyWord::Grow:
                    if (targetMinion != nullptr) {
                        manager->grow(*targetMinion, effect.amount, effect.amount, 0); // 示例
                    }
                    break;
                case Spell::KeyWord::Crystal:
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
