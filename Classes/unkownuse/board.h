#pragma once

#include <vector>
#include "entity/Hero.h"
#include "Hand.h"
#include "Deck.h"
#include "Crystal.h"
#include "entity/Minion.h"

// 支持 创建自定义牌库
// 将卡组进行编码 编码后传入board中
// board进行解码，将卡组还原

class board {
public:
    board(HeroType ownhero, HeroType oppohero) {
        
    }
    

public:
    Hero* ownhero;                  // 己方英雄
    Hero* oppohero;                 // 敌方英雄
    Hand* hand;                     // 手牌
    Deck* deck;                     // 牌库
    Crystal* crystal;               // 水晶
    std::vector<Minion*> minions;   // 随从

};