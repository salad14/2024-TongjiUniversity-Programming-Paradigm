/****************************************************************
* ����ҳ��Ķ���
* Author: Lee
 ****************************************************************/

#include "cocos2d.h"

class BoardScene : public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void menuCloseCallback(Ref* pSender);

	CREATE_FUNC(BoardScene);

	bool createCards();

	bool onMouseDown(cocos2d::Event* event, int cardIndex);

private:
	std::vector<cocos2d::Sprite*> cards; // �洢���п�Ƭ
	cocos2d::Sprite* selectedCard; // ��ǰѡ�еĿ�Ƭ
};