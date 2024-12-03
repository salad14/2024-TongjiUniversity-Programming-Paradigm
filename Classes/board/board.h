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
    Hero* ownhero;                  // 己方英雄
    Hero* oppohero;                 // 敌方英雄
    Hand* hand;                     // 手牌
    Deck* deck;                     // 牌库
    Crystal* crystal;               // 水晶
    std::vector<Minion*> minions;   // 随从

};