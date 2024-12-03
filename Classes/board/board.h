#pragma once

#include <vector>
#include "Hero.h"
#include "Hand.h"
#include "Deck.h"
#include "Crystal.h"
#include "Minion.h"

class board {
public:
	board();
    

public:
    Hero* ownhero;                  // ����Ӣ��
    Hero* oppohero;                 // �з�Ӣ��
    Hand* hand;                     // ����
    Deck* deck;                     // �ƿ�
    Crystal* crystal;               // ˮ��
    std::vector<Minion*> minions;   // ���

};