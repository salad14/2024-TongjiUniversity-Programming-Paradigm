// BoardScene.h
#ifndef __BOARD_SCENE_H__
#define __BOARD_SCENE_H__

#include "cocos2d.h"
#include "network/Photon_Lib.h"
#include "network/CocosUIListener.h" 
#include "network/CustomEventCodes.h"
#include "players/player.h"
#include "../card/ALLCard.h"
#include <cstdint> 
#include <vector>
#include <map>
USING_NS_CC;
class BoardScene : public cocos2d::Scene
{
public:
    // ���������ľ�̬����
    static cocos2d::Scene* createScene();

    // ��ʼ������
    virtual bool init();

    // �¼�����
    void onPhotonEvent(int eventCode, const ExitGames::Common::Hashtable& parameters);

    // ������
    CREATE_FUNC(BoardScene);

private:
    // Photon �� UI ���
    PhotonLib* photonLib;
    CocosUIListener* cocosUIListener;

    // UI Ԫ��
    cocos2d::DrawNode* dropArea;

    // ������
    int localPlayerNumber;

    players::Player* player1;
    players::Player* player2;

    int currentPlayerNumber;
    bool isLocalPlayerTurn;   // ��ǰ�غ��Ƿ����ڱ������

    bool initialHandsDistributed = false;
    bool handsInitialized = false;

    // ���ƹ���
    std::vector<cocos2d::Sprite*> localPlayerCards; // ���еĿ���
    std::map<cocos2d::Sprite*, cocos2d::Vec2> cardOriginalPositions; // ���Ƶ�ԭʼλ��
    std::vector<Sprite*> localplayedCards;        // ��������Ѵ���Ŀ���
    std::vector<Sprite*> oppentplayedCards;        // �Է�����Ѵ���Ŀ���

    // ��ǰѡ�еĿ���
    cocos2d::Sprite* selectedCard;
    cocos2d::Sprite* hoveredCard;

    // �����Ϣ UI
    cocos2d::Label* localPlayerHealth;
    cocos2d::Label* localPlayerMana;
    cocos2d::Label* opponentPlayerHealth;
    cocos2d::Label* opponentPlayerMana;
    cocos2d::Label* turnIndicator;

    // �غϽ�����ť
    cocos2d::MenuItemImage* endTurnButton;

    // ��������
    void cancelCallback(cocos2d::Ref* pSender);

    void createDropArea();
    void checkDropArea();

   
    // �¼��������ص�
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void onMouseMove(cocos2d::Event* event);


    void returnCardToOriginalPosition(Sprite* card);

    // ���ž���
    void scaleSprite(cocos2d::Sprite* sprite, float scale);

    // ��ҹ���
    void initPlayers();
    void createPlayerUI();
    void updatePlayerUI();
    void switchTurn();

    // ���ƹ���
    void removeCard(cocos2d::Sprite* sprite);
    void updatePlayedCardsPosition();

    void addCardToBattlefield(int playerNumber, int cardNumber);

    // �¼�����
    void sendPlayCardEvent(PlayerNumber playerNumber, CardNumber cardNumber);
    void sendTurnStartEvent();

    // Photon �¼�����
    void handlePlayCard(const ExitGames::Common::Hashtable& parameters);
    void handleTurnStart(const ExitGames::Common::Hashtable& parameters);


    // ��Ϸ����
    void endGame(players::Player* winner);

    // �����߼�
    void update(float dt) override;

    // �ַ���ʼ����
    void distributeInitialHands();
    void addCardToLocalPlayer(CardNumber cardNumber);

    // ��ȡ���Ƶķ���
    int getCardCost(int cardNumber);

    // �������������ݿ���ID���Ҿ���
    cocos2d::Sprite* findCardByID(int cardID);

    // ��������
    virtual ~BoardScene();
};

#endif // __BOARD_SCENE_H__
