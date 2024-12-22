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
        // 构造对象
        cardSprite* sprite = new (std::nothrow) cardSprite();
        if (sprite && sprite->initWithFile("cards/" + std::to_string(card->dbfId) + ".png")) {
            // 调整大小
            Size originalSize = sprite->getContentSize();
            float scaleX = desiredSize.width / originalSize.width;
            float scaleY = desiredSize.height / originalSize.height;
            float scale = std::min(scaleX, scaleY); // 保持比例
            sprite->setScale(scale);

            sprite->autorelease();
            sprite->card = card;
            return sprite;
        }
        // 如果初始化失败，删除对象并返回 nullptr
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

    // 本地玩家
    int localPlayerNumber; // 1, 2

    players::Player* player1;
    players::Player* player2;

    int currentPlayerNumber;
    bool isLocalPlayerTurn;   // 当前回合是否属于本地玩家

    bool initialHandsDistributed = false;
    bool handsInitialized = false;

    // 手牌管理
    std::vector<cardSprite*> localPlayerCards; // 手中的卡牌
    std::map<cardSprite*, cocos2d::Vec2> cardOriginalPositions; // 卡牌的原始位置
    std::vector<cardSprite*> localMinionCard;        // 本地玩家场上的随从牌
    std::vector<cardSprite*> oppentMinionCard;        // 对方玩家场上的随从牌

    // 卡牌状态
    cardSprite* selectedCard;//选中
    cardSprite* hoveredCard;//悬停
    cardSprite* attackingCard;//攻击
    DrawNode* attackIndicator;//被攻击

    // 卡牌属性结构
    //struct CardInfo {
    //    int health;
    //    int attack;
    //    int cost;
    //};

    //std::map<cardSprite*, CardInfo> cardStats;

    // 玩家信息 UI
    cocos2d::Label* localPlayerHealth;
    cocos2d::Label* localPlayerMana;
    cocos2d::Label* opponentPlayerHealth;
    cocos2d::Label* opponentPlayerMana;
    cocos2d::Label* turnIndicator;

    // 回合结束按钮
    cocos2d::MenuItemImage* endTurnButton;

    //////////////////////////
    //std::map<cocos2d::Sprite*, std::shared_ptr<CardBase>> cardMap; // 卡牌精灵和卡牌对象的映射
    // JSONManager manager;

    // 方法声明
    void cancelCallback(cocos2d::Ref* pSender);

    void createDropArea();
    void checkDropArea();

   
    // 事件监听器回调
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    void onMouseMove(cocos2d::Event* event);
    void scaleSprite(cardSprite* sprite, float scale);

    // 玩家管理
    void initPlayers();
    void createPlayerUI();
    void updatePlayerUI(); // 没有完成
    void switchTurn();

    // 卡牌管理
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
    // void handleMinionAttackMinion(int attacker, int defender); // 改为index索引用来绑定  方便双方同时处理 需要保证两边同步index相同
    void removeCardWithAnimation(cardSprite* card);
    void handleMinionAttackHero(); // 纯UI函数

    // 事件发送
    void sendPlay_MinionCardEvent(PlayerNumber playerNumber, CardNumber cardNumber);
    void sendPlay_SpellCardEvent(PlayerNumber playerNumber, CardNumber dbfID);
    void sendSpellAttackEvent(PlayerNumber attackPlayer, int defenderIndex, int damage);
    //void sendDrawCardEvent(PlayerNumber playerNumber);
    void sendMinionAttackEvent(PlayerNumber playerNumber, int attackerIndex, int defenderIndex);
    void sendTurnStartEvent();

    // Photon 事件处理
    void handle_PlayMinionCard(const ExitGames::Common::Hashtable& parameters);
    void handle_PlaySpellCard(const ExitGames::Common::Hashtable& parameters);
    void handle_SpellAttackEvent(const ExitGames::Common::Hashtable& parameters);
    //void handle_DrawCardEvent(const ExitGames::Common::Hashtable& parameters);
    void handle_MinionAttackEvent(const ExitGames::Common::Hashtable& parameters);  
    void handle_TurnStart(const ExitGames::Common::Hashtable& parameters);
    

    // 游戏结束
    void endGame(players::Player* winner);

    // 更新逻辑
    void update(float dt) override;

    // 分发初始手牌
    void distributeInitialHands();
    void addCardToLocalPlayer(std::shared_ptr<CardBase> card);


    // 辅助函数
    // 获取卡牌的费用
    //int getCardCost(std::shared_ptr<CardBase> card);
    //int getCardCost(int cardNumber);

    // 检测随从死亡  同时释放资源和展示动画
    bool checkMinionDie(cardSprite* minion);

    // 获取场上随从牌的index
    int get_localMinionIndex(cardSprite* minion);
    int get_opponentMinionIndex(cardSprite* minion);

    //void initEnemyCards();

    // 辅助方法：根据卡牌ID查找精灵
    cardSprite* findCardByID(int cardID);
    // 析构函数
    virtual ~BoardScene();
};

#endif // __BOARD_SCENE_H__
