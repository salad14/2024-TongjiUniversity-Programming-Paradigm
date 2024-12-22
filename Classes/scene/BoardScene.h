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

static JSONManager jsonmanager("cards/json/cards.json");

class cardSprite : public cocos2d::Sprite {
public:
    static cardSprite* createWithCard(const std::shared_ptr<CardBase>& card, const Size& desiredSize = Size(200, 250)) {
        // �������
        cardSprite* sprite = new (std::nothrow) cardSprite();
        if (sprite && sprite->initWithFile("cards/" + std::to_string(card->dbfId) + ".png")) {
            // ������С
            Size originalSize = sprite->getContentSize();
            float scaleX = desiredSize.width / originalSize.width;
            float scaleY = desiredSize.height / originalSize.height;
            float scale = std::min(scaleX, scaleY); // ���ֱ���
            sprite->setScale(scale);

            sprite->autorelease();
            sprite->card = card;
            return sprite;
        }
        // �����ʼ��ʧ�ܣ�ɾ�����󲢷��� nullptr
        CC_SAFE_DELETE(sprite);
        return nullptr;
    }

    void getDamage(int damage) { this->currentHealth -= damage; }

public:
    std::shared_ptr<CardBase> card;
    int currentHealth = 0;
    int currentAttack = 0;
};

//class minionSprite : public cocos2d::Sprite {
//public:
//    static minionSprite* creatWithCard(const std::shared_ptr<CardBase>& card, const Size& desiredSize = Size(200, 250)) {
//
//    }
//public:
//    int currentHealth;  // > 0
//    int attack;
//};

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

    // �������
    int localPlayerNumber; // 1, 2

    players::Player* player1;
    players::Player* player2;

    int currentPlayerNumber;
    bool isLocalPlayerTurn;   // ��ǰ�غ��Ƿ����ڱ������

    bool initialHandsDistributed = false;
    bool handsInitialized = false;

    // ���ƹ���
    std::vector<cardSprite*> localPlayerCards; // ���еĿ���
    std::map<cardSprite*, cocos2d::Vec2> cardOriginalPositions; // ���Ƶ�ԭʼλ��
    std::vector<cardSprite*> localMinionCard;        // ������ҳ��ϵ������
    std::vector<cardSprite*> oppentMinionCard;        // �Է���ҳ��ϵ������

    // ����״̬
    cardSprite* selectedCard;//ѡ��
    cardSprite* hoveredCard;//��ͣ
    cardSprite* attackingCard;//����
    DrawNode* attackIndicator;//������

    // �������Խṹ
    //struct CardInfo {
    //    int health;
    //    int attack;
    //    int cost;
    //};

    //std::map<cardSprite*, CardInfo> cardStats;

    // �����Ϣ UI
    cocos2d::Label* localPlayerHealth;
    cocos2d::Label* localPlayerMana;
    cocos2d::Label* opponentPlayerHealth;
    cocos2d::Label* opponentPlayerMana;
    cocos2d::Label* turnIndicator;

    // �غϽ�����ť
    cocos2d::MenuItemImage* endTurnButton;

    //////////////////////////
    //std::map<cocos2d::Sprite*, std::shared_ptr<CardBase>> cardMap; // ���ƾ���Ϳ��ƶ����ӳ��
    // JSONManager manager;

    // ��������
    void cancelCallback(cocos2d::Ref* pSender);

    void createDropArea();
    void checkDropArea();

   
    // �¼��������ص�
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void onMouseMove(cocos2d::Event* event);
    void scaleSprite(cardSprite* sprite, float scale);

    // ��ҹ���
    void initPlayers();
    void createPlayerUI();
    void updatePlayerUI(); // û�����
    void switchTurn();

    // ���ƹ���
    void removeCard(cardSprite* sprite);
    void updatePlayedCardsPosition();
    void updateEnemyCardsPosition();
    void addCardStats(cardSprite* card);
    void updateCardStats(cardSprite* card);
    void returnCardToHand(cardSprite* card);
    void add_NewCardToBattlefield(int playerNumber, int cardNumber);
    void add_HandCardToBattlefield(cardSprite* minion);
    void createAttackIndicator(const Vec2& startPos);
    void attackmove(PlayerNumber player, int attackerIndex, int defenderIndex);
    // void handleMinionAttackMinion(int attacker, int defender); // ��Ϊindex����������  ����˫��ͬʱ���� ��Ҫ��֤����ͬ��index��ͬ
    void removeCardWithAnimation(cardSprite* card);
    void handleMinionAttackHero(); // ��UI����

    // �¼�����
    void sendPlay_MinionCardEvent(PlayerNumber playerNumber, CardNumber cardNumber);
    void sendPlay_SpellCardEvent(PlayerNumber playerNumber, CardNumber dbfID);
    void sendSpellAttackEvent(PlayerNumber attackPlayer, int defenderIndex, int damage);
    //void sendDrawCardEvent(PlayerNumber playerNumber);
    void sendMinionAttackEvent(PlayerNumber playerNumber, int attackerIndex, int defenderIndex);
    void sendTurnStartEvent();

    // Photon �¼�����
    void handle_PlayMinionCard(const ExitGames::Common::Hashtable& parameters);
    void handle_PlaySpellCard(const ExitGames::Common::Hashtable& parameters);
    void handle_SpellAttackEvent(const ExitGames::Common::Hashtable& parameters);
    //void handle_DrawCardEvent(const ExitGames::Common::Hashtable& parameters);
    void handle_MinionAttackEvent(const ExitGames::Common::Hashtable& parameters);  
    void handle_TurnStart(const ExitGames::Common::Hashtable& parameters);
    

    // ��Ϸ����
    void endGame(players::Player* winner);

    // �����߼�
    void update(float dt) override;

    // �ַ���ʼ����
    void distributeInitialHands();
    void addCardToLocalPlayer(std::shared_ptr<CardBase> card);


    // ��������
    // ��ȡ���Ƶķ���
    //int getCardCost(std::shared_ptr<CardBase> card);
    //int getCardCost(int cardNumber);

    // ����������  ͬʱ�ͷ���Դ��չʾ����
    bool checkMinionDie(cardSprite* minion);

    // ��ȡ��������Ƶ�index
    int get_localMinionIndex(cardSprite* minion);
    int get_opponentMinionIndex(cardSprite* minion);

    //void initEnemyCards();

    // �������������ݿ���ID���Ҿ���
    cardSprite* findCardByID(int cardID);
    // ��������
    virtual ~BoardScene();
};

#endif // __BOARD_SCENE_H__
