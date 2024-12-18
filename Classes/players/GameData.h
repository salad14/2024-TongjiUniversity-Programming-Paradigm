// GameData.h
#ifndef _GAMEDATA_H_
#define _GAMEDATA_H_

#include "Player.h"

namespace players {
    class Player;
}

class GameData {
public:
    // 获取单例实例
    static GameData& getInstance();

    // 获取玩家对象
    players::Player* getPlayer1();
    players::Player* getPlayer2();

    // 获取唯一的卡牌编号
    CardNumber getUniqueCardNumber();

private:
    // 私有构造函数和析构函数
    GameData();
    ~GameData();

    // 禁用拷贝构造和赋值操作
    GameData(const GameData&) = delete;
    GameData& operator=(const GameData&) = delete;

    players::Player* player1;
    players::Player* player2;

    CardNumber uniqueCardNumber;
};

#endif // _GAMEDATA_H_
