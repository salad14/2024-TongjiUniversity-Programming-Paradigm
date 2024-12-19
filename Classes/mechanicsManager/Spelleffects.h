// Classes/mechanicsManager/Spelleffects

#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>
#include "entity/Minion.h"

class Manager {
public:
    // ����Ŀ��
    void frozen(const std::vector<int>& targetList) {
        for (int targetId : targetList) {
            auto it = minions.find(targetId);
            if (it != minions.end()) {
                it->second->setFrozen(true);
            }
        }
    }

    // ��Ŀ������˺�
    void damage(const std::vector<int>& targetList, int damage) {
        for (int targetId : targetList) {
            auto it = minions.find(targetId);
            if (it != minions.end()) {
                it->second->takeDamage(damage);
            }
        }
    }

    // �鿨
    void draw(int num) {
        std::cout << "Drawing " << num << " card(s)." << std::endl;
    }

    // ��ǿ���
    void grow(Minion& target, int health, int attack, int crystal) {
        target.currentHealth += health;
        target.attack += attack;
        std::cout << "Minion " << target.name << " grows by +" << health << " health and +" << attack << " attack." << std::endl;
    }

    // ����ˮ��
    void crystal(int round, int crystal) {
        if (round == -1) {
            // �������
            std::cout << "Handling overload with " << crystal << " crystals." << std::endl;
        }
        else if (round == 0) {
            // �����غ���ʱ����ˮ��
            std::cout << "Adding " << crystal << " temporary crystals for this round." << std::endl;
        }
        else if (round == 1) {
            // ������������ˮ��
            std::cout << "Adding " << crystal << " permanent crystals." << std::endl;
        }
    }

    // �����ӵ�������
    void addMinion(int id, std::shared_ptr<Minion> minion) {
        minions[id] = minion;
    }

private:
    std::unordered_map<int, std::shared_ptr<Minion>> minions;

    void removeMinion(int id) {
        std::cout << "Minion " << id << " has been removed from the game." << std::endl;
        minions.erase(id);
    }
};