/****************************************************************
 * �Զ��忨�����Ķ��壨���ԣ�
 * Author:   Lee
 ****************************************************************/

#ifndef __EDIT_GROUP_SCENE_H__
#define __EDIT_GROUP_SCENE_H__
#include "cocos2d.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
class EditGroupScene : public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(EditGroupScene);
private:
	cocos2d::ui::ScrollView* m_scrollView;
	cocos2d::Node* m_cardContainer;

	// ������Ա���ֲ���
	cocos2d::Node* m_selectedContainer;
	std::vector<int> m_selectedCardIds;

	void initScrollView();
	void initSelectedArea();
	void initButtons();

	void onCardSelected(int cardId);
	void onCardRemoved(int cardId);
	void onFinishButtonClicked(Ref* sender);

	cocos2d::Sprite* m_draggedSprite;
	cocos2d::Vec2 m_dragOffset;
	//void initDragAndDrop();

};
#endif // __EDIT_GROUP_SCENE_H_