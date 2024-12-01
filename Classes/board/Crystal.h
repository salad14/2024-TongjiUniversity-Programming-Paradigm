#pragma once

#include "cocos2d.h"

const int MAX_CRYSTAL = 10;

class Crystal : public cocos2d::Sprite{
public:
	Crystal() = default;

	// ������������
	Crystal& operator++(int) {
        crystalNum++;
	}

	const int getCrystalNum() { return crystalNum; }

private:
	int crystalNum = 0;

};