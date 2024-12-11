/****************************************************************
 * 玩家类的定义
 * Author:  Lee
 * 注：暂时用于ui界面开发测试，待完善
 ****************************************************************/
#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "cocos2d.h"
USING_NS_CC;
using namespace std;
#include "player.h"
#include "heroes/Hero.h"
#include "cards/card.h"


class Player 
{
public:
    // 构造函数
    Player(const string nickname);
    std::vector<Sprite*> playerCards;  // 玩家携带的卡组
    // 自定义玩家携带的卡组
    void setPlayerCards();
    int health = 30;   // 当前生命值
    int money = 0;     // 当前费
    int maxmoney = 1;  // 最大费
private:
    string nickname;
    int maxhealth = 30;// 最大生命值
    
};
#endif // !_PLAYER_H_
