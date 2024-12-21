// GameData.cpp
#include "GameData.h"
#include "Player.h"
#include "cocos2d.h"

// ˽�й��캯��
GameData::GameData() : uniqueCardNumber(1000) { // ��ʼ�����Ʊ�Ŵ�1000��ʼ
    // ��ʼ����ң����� *this
    player1 = new players::Player(1, "Player1", *this);
    player2 = new players::Player(2, "Player2", *this);
    CCLOG("GameData initialized with Player1 and Player2.");
}

// ��������
GameData::~GameData() {
    delete player1;
    delete player2;
    CCLOG("GameData destroyed and players cleaned up.");
}

// ��ȡΨһ�Ŀ��Ʊ��
CardNumber GameData::getUniqueCardNumber() {
    return uniqueCardNumber++;
}
