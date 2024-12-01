#pragma once

#include "cocos2d.h"

class BoardScene : public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void menuCloseCallback(Ref* pSender);

	CREATE_FUNC(BoardScene);

private:
};