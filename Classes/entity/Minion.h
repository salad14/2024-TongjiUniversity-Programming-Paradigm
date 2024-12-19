// Minion.h
#pragma once

#include <string>
#include <iostream>
#include "../card/MinionCard.h"

class Minion {
public:
    int attack;
    int health;
    int currentHealth;
    std::string name;
    MinionkeyWord::keyWord keywords;
    bool frozen;
    bool taunt;
    bool windfury;

    // ���캯����ͨ�� MinionCard ��ʼ�� Minion
    Minion(int dbfId, const std::string& name, int attack, int health, MinionkeyWord::keyWord keywords = MinionkeyWord::None)
        : attack(attack), health(health), currentHealth(health), name(name), keywords(keywords),
        frozen(false), taunt(false), windfury(false) {
        // ���� keywords ��ʼ������
        if (static_cast<int>(keywords) & static_cast<int>(MinionkeyWord::Taunt)) {
            taunt = true;
        }
        if (static_cast<int>(keywords) & static_cast<int>(MinionkeyWord::Windfury)) {
            windfury = true;
        }
    }

    // ����ս��
    void triggerBattlecry() {
        //std::cout << name << "'s Battlecry triggered!" << std::endl;
    }

    // ����Ŀ��
    void attackTarget(Minion* target) {
        if (target) {
            //std::cout << name << " is attacking " << target->name << "!" << std::endl;
            target->takeDamage(attack);
        }
    }

    // �����˺�
    void takeDamage(int damage) {
        currentHealth -= damage;
        //std::cout << name << " takes " << damage << " damage. Current health: " << currentHealth << std::endl;
        if (currentHealth <= 0) {
            die();
        }
    }

    // ��������
    void die() {
        //std::cout << name << " has died!" << std::endl;
        // ����Ч���� keyWordManager ����
    }

    // ���ö���״̬
    void setFrozen(bool state) {
        frozen = state;
        if (frozen) {
            //std::cout << name << " is frozen." << std::endl;
        }
        else {
            //std::cout << name << " is unfrozen." << std::endl;
        }
    }

    bool isFrozen() const {
        return frozen;
    }

    // ������ӵ�״̬����ʾ
    void updateStatus() const {
        //std::cout << "Minion " << name << " has " << currentHealth << " HP remaining." << std::endl;
    }

    // ��ǿ������������ֵ
    void grow(int healthIncrease, int attackIncrease) {
        currentHealth += healthIncrease;
        health += healthIncrease;
        attack += attackIncrease;
        //std::cout << name << " grows by +" << attackIncrease << " attack and +" << healthIncrease << " health." << std::endl;
    }
};
