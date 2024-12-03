#pragma once

#include <functional>
#include <string>
#include <iostream>
#include "card.h"
#include "cocos2d.h"

class Minion : public cocos2d::Sprite {
public:
    int attack;
    int health;
    int currentHealth;
    std::string name;
    BattlecryEffect battlecryEffect;
    MinionCard* minionCard;  // �洢 MinionCard ��ʵ��

    // ���캯����ͨ�� MinionCard ��ʼ�� Minion
    Minion(MinionCard* card) {
        minionCard = card;
        attack = card->attack;
        health = card->health;
        currentHealth = health;
        name = card->name;

        // ��ʼ��ս��Ч��
        battlecryEffect = card->battlecryEffect;

        // ͨ�� cocos2d::Sprite ��ʼ�����飨������һ��ͼƬ�ļ���ʾ��ӣ�
        this->initWithFile("minion_image.png"); // ��������һ����ӵ�ͼƬ

        // ���þ���ĳߴ��
        this->setContentSize(cocos2d::Size(100, 100));  // �趨��С
    }

    // ����ս��
    void triggerBattlecry() {
        if (battlecryEffect) {
            std::cout << name << "'s Battlecry triggered!" << std::endl;
            battlecryEffect();  // ����ս��Ч��
        }
    }

    // ����Ŀ��
    void attackTarget(Minion* target) {
        if (target) {
            std::cout << name << " is attacking " << target->name << "!" << std::endl;
            target->takeDamage(attack);
        }
    }

    // �����˺�
    void takeDamage(int damage) {
        currentHealth -= damage;
        if (currentHealth <= 0) {
            die();
        }
    }

    // ��������
    void die() {
        std::cout << name << " has died!" << std::endl;
        // ��������ʱ�������߼����紥������Ч��
        if (static_cast<int>(minionCard->keywords) & static_cast<int>(mKeyword::Deathrattle)) {
            std::cout << "Deathrattle triggered!" << std::endl;
            // ����Ч��
        }
        // �� cocos2d ���Ƴ�����
        this->removeFromParent();
    }

    // ������ӵ�״̬����ʾ
    void update() {
        // �������״̬��������ʾʣ��������
        std::cout << "Minion " << name << " has " << currentHealth << " HP remaining." << std::endl;
    }

    // ���������繥����������ŭ�ȿ����ڴ����...
};