/****************************************************************
 * 人机对战棋盘的定义
 * Author: Lee
 ****************************************************************/

#ifndef __OFFLINE_BOARD_SCENE_H__
#define __OFFLINE_BOARD_SCENE_H__
#include "cocos2d.h"
#include <string>
#include <vector>
USING_NS_CC;
using namespace std;

class Player
{
public:
    // 构造函数
    Player(const string s)
    {
        nickname = s;
        return;
    }
    //初始化卡组函数（仅在离线模式使用）
    void Player::setPlayerCards()
    {
        for (int i = 0; i < 30; ++i) {
            auto sprite = Sprite::create("card1.png");
            this->playerCards.push_back(sprite);
        }
    }
    std::vector<Sprite*> playerCards;  // 玩家携带的卡组
    int health = 30;   // 当前生命值
    int money = 0;     // 当前费
    int maxmoney = 1;  // 最大费
private:
    string nickname;
    int maxhealth = 30;// 最大生命值

};

class BoardSceneOffline : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(BoardSceneOffline);
private:
    // 基础UI元素
    void cancelCallback(Ref* pSender);
    void createDropArea();
    void checkDropArea();
    DrawNode* dropArea;
    // 卡牌管理
    std::vector<Sprite*> dragCards;        // 手牌区域的卡牌
    std::vector<Sprite*> playedCards;      // 己方场上的卡牌
    std::vector<Sprite*> enemyPlayedCards; // 敌方场上的卡牌（随从）
    std::map<Sprite*, Vec2> cardOriginalPositions;// 用于记录卡牌位置

    // 卡牌状态
    Sprite* selectedCard;//选中
    Sprite* hoveredCard;//悬停
    Sprite* attackingCard;//攻击
    DrawNode* attackIndicator;//被攻击

    // 卡牌属性结构
    struct CardInfo {
        int health;
        int attack;
        int cost;
        string type;
    };
    std::map<Sprite*, CardInfo> cardStats;
    // 玩家相关
    Player* player1;
    Player* player2;
    bool isPlayer1Turn;
    Node* deckNode1;//己方初始卡组
    Node* deckNode2;//对方初始卡组
    // UI标签
    Label* player1Health;
    Label* player1Mana;
    Label* player2Health;
    Label* player2Mana;
    Label* turnIndicator;
    // 触摸事件处理
    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchMoved(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);
    void onMouseMove(Event* event);
    void scaleSprite(Sprite* sprite, float scale);
    // 游戏逻辑函数
    void removeCard(Sprite* sprite);
    void initPlayers();
    void createPlayerUI();
    void updatePlayerUI();
    void switchTurn();
    void initDecks();
    void drawCard();
    void updatePlayedCardsPosition();
    void updateEnemyCardsPosition();
    // 战斗相关函数
    //void initEnemyCards();
    void addCardStats(Sprite* card, int health, int attack, int cost, string type);
    void createAttackIndicator(const Vec2& startPos);
    void handleAttack(Sprite* attacker, Sprite* defender);
    void handleAttackToHero();
    void removeCardWithAnimation(Sprite* card);
    void updateCardStats(Sprite* card);
    void returnCardToHand(Sprite* card);
    void attackmove(int attackerIndex, int defenderIndex);//攻击动画
    void spellmove(Sprite* Target);     //法术牌攻击动画

    // 机器玩家函数
    void AIplay();

    // 在类定义中添加
    unordered_map<Sprite*, bool> hasAttacked; // 新增标记某张随从回合内是否已经攻击过
    Label* gameOverLabel;  // 游戏结束提示标签
    bool isGameOver;       // 游戏是否结束标志
    void checkGameOver();  // 检查游戏是否结束
    MenuItemImage* endTurnBtn; // 保存回合切换按钮的指针，用于在游戏结束时禁用

};
#endif // __OFFLINE_BOARD_SCENE_H__