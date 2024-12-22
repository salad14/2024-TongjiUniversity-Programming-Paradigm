/****************************************************************
 * 玩家类的实现
 * Author:  Lee
 * 注：暂时用于ui界面开发测试，待完善
 ****************************************************************/
#include "player.h"


// 构造函数
Player::Player(const string s)
{
	nickname = s;
	return;
}

//用户自定义卡组函数（待修改，此处仅引入30张最简单的手牌，仅测试用）
void Player::setPlayerCards()
{
	for (int i = 0; i < 30; ++i) {
		auto sprite = Sprite::create("card1.png");
		this->playerCards.push_back(sprite);
	}
}