/****************************************************************
* ����ҳ��Ķ���
* Author: Lee
 ****************************************************************/

#include "cocos2d.h"
USING_NS_CC;
#include "players/player.h"
#include <vector>
#include <map>
class BoardScene : public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	//void menuCloseCallback(Ref* pSender);

	CREATE_FUNC(BoardScene);

    virtual ~BoardScene() {
        cardOriginalPositions.clear();
        selectedCard = nullptr;
        hoveredCard = nullptr;
        dragCards.clear();
    }

private:
    std::vector<Sprite*> dragCards;                // �洢���п�
    std::map<Sprite*, Vec2> cardOriginalPositions;  // �洢ÿ�ſ���ԭʼλ��
    Sprite* selectedCard;            // ��ǰѡ�еľ���
    DrawNode* dropArea;              // ����������ʾ

    // ������������
    EventListenerMouse* mouseListener;
    Sprite* hoveredCard;             // ��ǰ�����ͣ�ľ���

    void createDropArea();              // ������������
    void checkDropArea();               // ����Ƿ��ڳ�������
    void addNewCard();               //  ����¾���
    void removeCard(Sprite* sprite); //  �Ƴ�����
    // ����¼�����
    void onMouseMove(Event* event);
    void scaleSprite(Sprite* sprite, float scale);// �Ŵ�Ԥ��ĳ����

    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchMoved(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);
   

   // void update(float dt);
};