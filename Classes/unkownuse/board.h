#pragma once

#include <vector>
#include "entity/Hero.h"
#include "Hand.h"
#include "Deck.h"
#include "Crystal.h"
#include "entity/Minion.h"

// ֧�� �����Զ����ƿ�
// ��������б��� �������board��
// board���н��룬�����黹ԭ

class board {
public:
    board(HeroType ownhero, HeroType oppohero) {
        
    }
    

public:
    Hero* ownhero;                  // ����Ӣ��
    Hero* oppohero;                 // �з�Ӣ��
    Hand* hand;                     // ����
    Deck* deck;                     // �ƿ�
    Crystal* crystal;               // ˮ��
    std::vector<Minion*> minions;   // ���

};