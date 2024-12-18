// BoardScene.h
#ifndef __BOARD_SCENE_H__
#define __BOARD_SCENE_H__

#include "cocos2d.h"
#include "network/Photon_Lib.h"
#include "network/CocosUIListener.h" 
#include "network/CustomEventCodes.h"
#include "players/player.h"
#include <cstdint> 
#include <vector>
#include <map>

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
    std::vector<cocos2d::Sprite*> playedCards; // �Ѵ���Ŀ���

    // ��ǰѡ�еĿ���
    cocos2d::Sprite* selectedCard;
    cocos2d::Sprite* hoveredCard;

    // �����Ϣ UI
    cocos2d::Label* player1Health;
    cocos2d::Label* player1Mana;
    cocos2d::Label* player2Health;
    cocos2d::Label* player2Mana;
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

    // �¼�����
    void sendPlayCardEvent(PlayerNumber playerNumber, CardNumber cardNumber);
    void sendTurnStartEvent();
    void sendDrawCardEvent(PlayerNumber playerNumber, CardNumber cardNumber);
    

    // Photon �¼�����
    void handlePlayCard(const ExitGames::Common::Hashtable& parameters);
    void handleEndTurn(const ExitGames::Common::Hashtable& parameters);
    void handleTurnStart(const ExitGames::Common::Hashtable& parameters);
    void handleDrawCard(const ExitGames::Common::Hashtable& parameters);

    // ��Ϸ����
    void endGame(players::Player* winner);

    // �����߼�
    void update(float dt) override;

    // �ַ���ʼ����
    void distributeInitialHands();

    // ��ȡ���Ƶķ���
    int getCardCost(cocos2d::Sprite* card);

    // �������������ݿ���ID���Ҿ���
    cocos2d::Sprite* findCardByID(int cardID);

    // ��������
    virtual ~BoardScene();
};

#endif // __BOARD_SCENE_H__
