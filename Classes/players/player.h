/****************************************************************
 * �����Ķ���
 * Author:  Lee
 * ע����ʱ����ui���濪�����ԣ�������
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
    // ���캯��
    Player(const string nickname);
    std::vector<Sprite*> playerCards;  // ���Я���Ŀ���
    // �Զ������Я���Ŀ���
    void setPlayerCards();
    int health = 30;   // ��ǰ����ֵ
    int money = 0;     // ��ǰ��
    int maxmoney = 1;  // ����
private:
    string nickname;
    int maxhealth = 30;// �������ֵ
    
};
#endif // !_PLAYER_H_
