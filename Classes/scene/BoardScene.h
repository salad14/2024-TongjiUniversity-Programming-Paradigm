/****************************************************************
 * ���̳����Ķ���
 * Author:  Lee
 * ע����ʱ����ui���濪�����ԣ�δ���������߼�
 ****************************************************************/

#ifndef __BOARD_SCENE_H__
#define __BOARD_SCENE_H__
#include "cocos2d.h"
#include "players/Player.h"
USING_NS_CC;
class BoardScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(BoardScene);
private:
    // ����UIԪ��
    void cancelCallback(Ref* pSender);
    void createDropArea();
    void checkDropArea();
    DrawNode* dropArea;
    // ���ƹ���
    std::vector<Sprite*> dragCards;        // ��������Ŀ���
    std::vector<Sprite*> playedCards;      // �������ϵĿ���
    std::vector<Sprite*> enemyPlayedCards; // �з����ϵĿ��ƣ���ӣ�
    std::map<Sprite*, Vec2> cardOriginalPositions;// ���ڼ�¼����λ��

    // ����״̬
    Sprite* selectedCard;//ѡ��
    Sprite* hoveredCard;//��ͣ
    Sprite* attackingCard;//����
    DrawNode* attackIndicator;//������

    // �������Խṹ
    struct CardInfo {
        int health;
        int attack;
        int cost;
        string type;
    };
    std::map<Sprite*, CardInfo> cardStats;
    // ������
    Player* player1;
    Player* player2;
    bool isPlayer1Turn;
    Node* deckNode1;//������ʼ����
    Node* deckNode2;//�Է���ʼ����
    // UI��ǩ
    Label* player1Health;
    Label* player1Mana;
    Label* player2Health;
    Label* player2Mana;
    Label* turnIndicator;
    // �����¼�����
    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchMoved(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);
    void onMouseMove(Event* event);
    void scaleSprite(Sprite* sprite, float scale);
    // ��Ϸ�߼�����
    void removeCard(Sprite* sprite);
    void initPlayers();
    void createPlayerUI();
    void updatePlayerUI();
    void switchTurn();
    void initDecks();
    void drawCard();
    void updatePlayedCardsPosition();
    void updateEnemyCardsPosition();
    // ս����غ���
    //void initEnemyCards();
    void addCardStats(Sprite* card, int health, int attack, int cost, string type);
    void createAttackIndicator(const Vec2& startPos);
    void handleAttack(Sprite* attacker, Sprite* defender);
    void handleAttackToHero();
    void removeCardWithAnimation(Sprite* card);
    void updateCardStats(Sprite* card);
    void returnCardToHand(Sprite* card);
    void attackmove(int attackerIndex, int defenderIndex);//��������
    void spellmove(Sprite* Target);     //�����ƹ�������

    // ������Һ���
    void AIplay();

    // ���ඨ�������
    bool hasAttackedThisTurn = false;  // ��¼���غ��Ƿ��Ѿ�����ӹ���
    Label* gameOverLabel;  // ��Ϸ������ʾ��ǩ
    bool isGameOver;       // ��Ϸ�Ƿ������־
    void checkGameOver();  // �����Ϸ�Ƿ����
   
};
#endif // __BOARD_SCENE_H__