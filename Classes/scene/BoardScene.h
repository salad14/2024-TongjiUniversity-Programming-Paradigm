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
    std::vector<Sprite*> dragCards;                 // ����������Ŀ���
    std::map<Sprite*, Vec2> cardOriginalPositions;  // �洢����������ÿ�ſ���ԭʼλ��
    Sprite* selectedCard;            // ��ǰѡ�еľ���
    DrawNode* dropArea;              // ����������ʾ

    // ������������
    EventListenerMouse* mouseListener;
    Sprite* hoveredCard;             // ��ǰ�����ͣ�ľ���

    void cancelCallback(Ref* pSender);
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
   
    Player* player1;    // �������
    Player* player2;    // �з����
    bool isPlayer1Turn; // ��ǰ�غϱ�־

    // UIԪ��
    Label* player1Health;
    Label* player1Mana;
    Label* player2Health;
    Label* player2Mana;
    Label* turnIndicator;  // �غ�ָʾ��

    void initPlayers();    // ��ʼ�����
    void updatePlayerUI(); // ���������Ϣ��ʾ
    void switchTurn();     // �л��غ�
    void createPlayerUI(); // �������UI
   // void update(float dt);

    // ������Ա����
    vector<Sprite*> playedCards;         // �Ѵ���Ŀ���
    Node* deckNode1;                     // �ƶ���ʾ�ڵ�1
    Node* deckNode2;                     // �ƶ���ʾ�ڵ�2

    void initDecks();                    // �����û����� ��ʼ���ƶ�
    void drawCard();                     // ����
    void updatePlayedCardsPosition();    // �����Ѵ�����Ƶ�λ��
};