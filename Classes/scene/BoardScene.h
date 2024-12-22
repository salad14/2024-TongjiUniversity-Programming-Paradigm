/****************************************************************
 * 棋盘场景的定义
 * Author:  Lee
 * 注：暂时用于ui界面开发测试，未加入联机逻辑
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
    bool hasAttackedThisTurn = false;  // 记录本回合是否已经有随从攻击
    Label* gameOverLabel;  // 游戏结束提示标签
    bool isGameOver;       // 游戏是否结束标志
    void checkGameOver();  // 检查游戏是否结束
   
};
#endif // __BOARD_SCENE_H__