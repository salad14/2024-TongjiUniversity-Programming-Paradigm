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
    std::vector<Sprite*> dragCards;                // 存储所有卡
    std::map<Sprite*, Vec2> cardOriginalPositions;  // 存储每张卡的原始位置
    Sprite* selectedCard;            // 当前选中的精灵
    DrawNode* dropArea;              // 中央区域显示

    // 添加鼠标监听相关
    EventListenerMouse* mouseListener;
    Sprite* hoveredCard;             // 当前鼠标悬停的精灵

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
   

   // void update(float dt);
};