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

    // ��ȡ����
    const std::string& getName() const { return card->getName(); }

    // ��ȡ����
    const std::string& getDescription() const { return card->getDescription(); }

    // ��ȡ�����÷���
    int getCost() const { return currentCost; }
    void setCost(int newCost) { currentCost = newCost; }

    // ִ�з���Ч��
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
                        manager->grow(*targetMinion, effect.amount, effect.amount, 0); // ʾ��
                    }
                    break;
                case Spell::KeyWord::Crystal:
                    manager->crystal(0, effect.amount);
                    break;
                    // ����Ч�����Ϳ���չ
                default:
                    break;
            }
        }
    }

private:
    std::shared_ptr<SpellCard> card; // ָ�� SpellCard �Ĺ���ָ��
    int currentCost; // ��̬����
};
