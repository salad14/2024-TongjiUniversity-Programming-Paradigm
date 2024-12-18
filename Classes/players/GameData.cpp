// GameData.cpp
#include "GameData.h"
#include "Player.h"
#include "cocos2d.h"

// 获取单例实例
GameData& GameData::getInstance() {
    static GameData instance;
    return instance;
}

// 私有构造函数
GameData::GameData() : uniqueCardNumber(1000) { // 初始化卡牌编号从1000开始
    // 初始化玩家，传递 *this
    player1 = new players::Player(1, "Player1", *this);
    player2 = new players::Player(2, "Player2", *this);
    CCLOG("GameData initialized with Player1 and Player2.");
}

// 析构函数
GameData::~GameData() {
    delete player1;
    delete player2;
    CCLOG("GameData destroyed and players cleaned up.");
}

// 获取玩家1
players::Player* GameData::getPlayer1() {
    return player1;
}

// 获取玩家2
players::Player* GameData::getPlayer2() {
    return player2;
}

// 获取唯一的卡牌编号
CardNumber GameData::getUniqueCardNumber() {
    return uniqueCardNumber++;
}
