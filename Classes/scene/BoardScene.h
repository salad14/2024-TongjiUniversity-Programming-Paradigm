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
    // 创建场景的静态方法
    static cocos2d::Scene* createScene();

    // 初始化方法
    virtual bool init();

    // 事件处理
    void onPhotonEvent(int eventCode, const ExitGames::Common::Hashtable& parameters);

    // 创建宏
    CREATE_FUNC(BoardScene);

private:
    // Photon 和 UI 相关
    PhotonLib* photonLib;
    CocosUIListener* cocosUIListener;

    // UI 元素
    cocos2d::DrawNode* dropArea;

    // 玩家相关
    int localPlayerNumber;

    players::Player* player1;
    players::Player* player2;

    int currentPlayerNumber;
    bool isLocalPlayerTurn;   // 当前回合是否属于本地玩家

    bool initialHandsDistributed = false;
    bool handsInitialized = false;

    // 手牌管理
    std::vector<cocos2d::Sprite*> localPlayerCards; // 手中的卡牌
    std::map<cocos2d::Sprite*, cocos2d::Vec2> cardOriginalPositions; // 卡牌的原始位置
    std::vector<cocos2d::Sprite*> playedCards; // 已打出的卡牌

    // 当前选中的卡牌
    cocos2d::Sprite* selectedCard;
    cocos2d::Sprite* hoveredCard;

    // 玩家信息 UI
    cocos2d::Label* player1Health;
    cocos2d::Label* player1Mana;
    cocos2d::Label* player2Health;
    cocos2d::Label* player2Mana;
    cocos2d::Label* turnIndicator;

    // 回合结束按钮
    cocos2d::MenuItemImage* endTurnButton;

    // 方法声明
    void cancelCallback(cocos2d::Ref* pSender);

    void createDropArea();
    void checkDropArea();

    // 事件监听器回调
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void onMouseMove(cocos2d::Event* event);

    // 缩放精灵
    void scaleSprite(cocos2d::Sprite* sprite, float scale);

    // 玩家管理
    void initPlayers();
    void createPlayerUI();
    void updatePlayerUI();
    void switchTurn();

    // 卡牌管理
    void removeCard(cocos2d::Sprite* sprite);
    void updatePlayedCardsPosition();

    // 事件发送
    void sendPlayCardEvent(PlayerNumber playerNumber, CardNumber cardNumber);
    void sendTurnStartEvent();
    void sendDrawCardEvent(PlayerNumber playerNumber, CardNumber cardNumber);
    

    // Photon 事件处理
    void handlePlayCard(const ExitGames::Common::Hashtable& parameters);
    void handleEndTurn(const ExitGames::Common::Hashtable& parameters);
    void handleTurnStart(const ExitGames::Common::Hashtable& parameters);
    void handleDrawCard(const ExitGames::Common::Hashtable& parameters);

    // 游戏结束
    void endGame(players::Player* winner);

    // 更新逻辑
    void update(float dt) override;

    // 分发初始手牌
    void distributeInitialHands();

    // 获取卡牌的费用
    int getCardCost(cocos2d::Sprite* card);

    // 辅助方法：根据卡牌ID查找精灵
    cocos2d::Sprite* findCardByID(int cardID);

    // 析构函数
    virtual ~BoardScene();
};

#endif // __BOARD_SCENE_H__
