/****************************************************************
 * ������ʵ��
 * Author:  Lee
 * ע����ʱ����ui���濪�����ԣ�������
 ****************************************************************/
#include "player.h"


// ���캯��
Player::Player(const string s)
{
	nickname = s;
	return;
}

//�û��Զ��忨�麯�������޸ģ��˴�������30����򵥵����ƣ��������ã�
void Player::setPlayerCards()
{
	for (int i = 0; i < 30; ++i) {
		auto sprite = Sprite::create("card1.png");
		this->playerCards.push_back(sprite);
	}
}