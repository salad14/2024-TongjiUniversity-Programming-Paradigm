// GameData.h
#ifndef _GAMEDATA_H_
#define _GAMEDATA_H_

#include "Player.h"

namespace players {
    class Player;
}

class GameData {
public:
    // ��ȡ����ʵ��
    static GameData& getInstance();

    // ��ȡ��Ҷ���
    players::Player* getPlayer1();
    players::Player* getPlayer2();

    // ��ȡΨһ�Ŀ��Ʊ��
    CardNumber getUniqueCardNumber();

private:
    // ˽�й��캯������������
    GameData();
    ~GameData();

    // ���ÿ�������͸�ֵ����
    GameData(const GameData&) = delete;
    GameData& operator=(const GameData&) = delete;

    players::Player* player1;
    players::Player* player2;

    CardNumber uniqueCardNumber;
};

#endif // _GAMEDATA_H_
