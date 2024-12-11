/****************************************************************
* 棋盘页面的定义
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
    std::vector<Sprite*> dragCards;                 // 待出牌区域的卡牌
    std::map<Sprite*, Vec2> cardOriginalPositions;  // 存储待出牌区域每张卡的原始位置
    Sprite* selectedCard;            // 当前选中的精灵
    DrawNode* dropArea;              // 中央区域显示

    // 添加鼠标监听相关
    EventListenerMouse* mouseListener;
    Sprite* hoveredCard;             // 当前鼠标悬停的精灵

    void cancelCallback(Ref* pSender);
    void createDropArea();              // 创建出牌区域
    void checkDropArea();               // 检查是否在出牌区域
    void addNewCard();               //  添加新精灵
    void removeCard(Sprite* sprite); //  移除卡牌
    // 鼠标事件处理
    void onMouseMove(Event* event);
    void scaleSprite(Sprite* sprite, float scale);// 放大预览某张牌

    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchMoved(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);
   
    Player* player1;    // 己方玩家
    Player* player2;    // 敌方玩家
    bool isPlayer1Turn; // 当前回合标志

    // UI元素
    Label* player1Health;
    Label* player1Mana;
    Label* player2Health;
    Label* player2Mana;
    Label* turnIndicator;  // 回合指示器

    void initPlayers();    // 初始化玩家
    void updatePlayerUI(); // 更新玩家信息显示
    void switchTurn();     // 切换回合
    void createPlayerUI(); // 创建玩家UI
   // void update(float dt);

    // 新增成员变量
    vector<Sprite*> playedCards;         // 已打出的卡牌
    Node* deckNode1;                     // 牌堆显示节点1
    Node* deckNode2;                     // 牌堆显示节点2

    void initDecks();                    // 根据用户卡组 初始化牌堆
    void drawCard();                     // 抽牌
    void updatePlayedCardsPosition();    // 更新已打出卡牌的位置
};