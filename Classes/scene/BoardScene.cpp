/****************************************************************
* 棋盘页面的实现
* Author: Lee
 ****************************************************************/

#include "BoardScene.h"
#include "proj.win32/Alluse.h"
#include "proj.win32/audioPlayer.h"

using namespace std;
USING_NS_CC;

Scene* BoardScene::createScene() {
    return BoardScene::create();
}

static void problemLoading(const char* filename) {
    printf("error, couldn't load file: %s\n", filename);
}

bool BoardScene::init() {
    if (!Scene::init()) {
        return false;
    }

    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto title_sprite = Sprite::create("board.png");
    if (title_sprite == nullptr) {
        problemLoading("board.png");
        return false;
    }
    else {
        // 设置图片的尺寸为窗口的尺寸
        title_sprite->setContentSize(visibleSize);
        // 将图片的位置调整为窗口的中心
        title_sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(title_sprite, 0);
    }

    // 创建添加卡牌的按钮(暂时用于测试)
    auto addButton = MenuItemImage::create
    (
        "button/addcard.png",
        "button/addcard.png",
        CC_CALLBACK_0(BoardScene::addNewCard, this)
    );
    addButton->setPosition(Vec2(1800, 100));
    auto menu = Menu::create(addButton, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);

    // 创建触摸监听器
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = CC_CALLBACK_2(BoardScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BoardScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BoardScene::onTouchEnded, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    selectedCard = nullptr;
    
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = CC_CALLBACK_1(BoardScene::onMouseMove, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    hoveredCard = nullptr;


    // 创建中央出牌区域
    createDropArea();

    // 启用update,按帧刷新
    //this->scheduleUpdate();

    return true;
}

// 创建中央出牌区域
void BoardScene::createDropArea() 
{
    dropArea = DrawNode::create();

    // 获取屏幕中心点
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 center = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

    // 绘制半透明的矩形区域（500x200）
    Vec2 vertices[] = 
    {
        Vec2(center.x - 500, center.y - 300),
        Vec2(center.x + 500, center.y - 300),
        Vec2(center.x + 500, center.y + 300),
        Vec2(center.x - 500, center.y + 300)
    };

    Color4F fillColor(0.5f, 0.5f, 0.5f, 0.3f); // 半透明灰色
    Color4F borderColor(1.0f, 1.0f, 1.0f, 0.8f); // 白色边框

    dropArea->drawPolygon(vertices, 4, fillColor, 2, borderColor);
    this->addChild(dropArea,1);
}

// 检查卡牌是否在投放区域
void BoardScene::checkDropArea() {
    if (!selectedCard)
        return;

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    Vec2 spritePos = selectedCard->getPosition();

    // 检查是否在投放区域内
    bool inDropArea =
        abs(spritePos.x - center.x) <= 500 &&
        abs(spritePos.y - center.y) <= 300;

    if (inDropArea)
    {
        // 直接设置卡牌颜色和描边
        selectedCard->setColor(Color3B(255, 100, 100));  // 使用较浅的红色

        // 创建一个描边效果
        if (!selectedCard->getChildByName("outline"))
        {
            auto cardSize = selectedCard->getContentSize();
            auto outline = DrawNode::create();
            outline->setName("outline");

            // 绘制粗边框，这里设置边框宽度为5像素
            Vec2 vertices[] = {
                Vec2(0, 0),
                Vec2(cardSize.width, 0),
                Vec2(cardSize.width, cardSize.height),
                Vec2(0, cardSize.height)
            };

            outline->drawPolygon(vertices, 4,
                Color4F(1, 0, 0, 0.0f),     // 填充颜色（透明）
                100.0f,                        // 边框宽度
                Color4F(1, 0, 0, 1.0f)      // 边框颜色（红色）
            );

            selectedCard->addChild(outline, 1);
        }
    }
    else
    {
        selectedCard->setColor(Color3B::WHITE);
        // 移除描边效果
        auto outline = selectedCard->getChildByName("outline");
        if (outline) {
            outline->removeFromParent();
        }
    }
}

//添加一张新的卡牌
void BoardScene::addNewCard() 
{
    // 创建新精灵
    auto sprite = Sprite::create("cardfortest.png");
    
    Vec2 originalPos(Vec2(CARD_REGION_X + dragCards.size() * (sprite->getContentSize().width + 30), CARD_REGION_Y));
    sprite->setPosition(originalPos);

    //加载牌音效
    audioPlayer("music/putcard.mp3", false);

    // 记录原始位置
    cardOriginalPositions[sprite] = originalPos;

    // 添加到场景和容器中
    this->addChild(sprite);
    dragCards.push_back(sprite);
    CCLOG("Current sprite count: %zu", dragCards.size());
}


//从出牌区域移除一张卡牌
void BoardScene::removeCard(Sprite* sprite) {
    if (!sprite) return;

    if (sprite == hoveredCard) {
        hoveredCard = nullptr;
    }

    // 从位置记录中移除
    cardOriginalPositions.erase(sprite);
    //加载牌音效
    audioPlayer("music/putcard.mp3", false);
    auto iter = std::find(dragCards.begin(), dragCards.end(), sprite);
    if (iter != dragCards.end()) {
        dragCards.erase(iter);
        sprite->stopAllActions();
        this->runAction(Sequence::create(
            DelayTime::create(0.1f),
            CallFunc::create([sprite]() {
                sprite->removeFromParent();
                }),
            nullptr
        ));
    }

    CCLOG("Current sprite count: %zu", dragCards.size());
}

// 鼠标移动检测
void BoardScene::onMouseMove(Event* event) {
    EventMouse* mouseEvent = dynamic_cast<EventMouse*>(event);
    Vec2 mousePos = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());

    // 检查鼠标是否悬停在某个精灵上
    Sprite* newHoveredSprite = nullptr;

    for (auto it = dragCards.rbegin(); it != dragCards.rend(); ++it) {
        Sprite* sprite = *it;
        Vec2 locationInNode = sprite->convertToNodeSpace(mousePos);
        Size s = sprite->getContentSize();
        Rect rect = Rect(0, 0, s.width, s.height);

        if (rect.containsPoint(locationInNode)) {
            newHoveredSprite = sprite;
            break;
        }
    }

    // 如果悬停的精灵发生变化
    if (hoveredCard != newHoveredSprite) {
        // 恢复之前悬停精灵的大小
        if (hoveredCard && hoveredCard != selectedCard) {
            scaleSprite(hoveredCard, 1.0f);
        }

        // 放大新的悬停精灵
        if (newHoveredSprite && newHoveredSprite != selectedCard) {
            scaleSprite(newHoveredSprite, 1.5f); // 放大1.5倍
        }

        hoveredCard = newHoveredSprite;
    }
}
void BoardScene::scaleSprite(Sprite* sprite, float scale) {
    // 使用动作实现平滑的缩放效果
    sprite->runAction(ScaleTo::create(0.1f, scale));
}

// 鼠标触摸检测
bool BoardScene::onTouchBegan(Touch* touch, Event* event)
{
    Vec2 touchLocation = touch->getLocation();

    // 从后向前检查（使最上层的精灵优先响应）
    for (auto it = dragCards.rbegin(); it != dragCards.rend(); ++it) {
        Sprite* sprite = *it;
        Vec2 locationInNode = sprite->convertToNodeSpace(touchLocation);
        Size s = sprite->getContentSize();
        Rect rect = Rect(0, 0, s.width, s.height);

        if (rect.containsPoint(locationInNode)) {
            selectedCard = sprite;
            // 开始拖动时恢复正常大小
            scaleSprite(sprite, 1.0f);
            return true;
        }
    }

    selectedCard = nullptr;
    return false;
}
void BoardScene::onTouchMoved(Touch* touch, Event* event) 
{
    if (selectedCard) 
    {
        selectedCard->setPosition(selectedCard->getPosition() + touch->getDelta());
    }
}
void BoardScene::onTouchEnded(Touch* touch, Event* event) 
{
    if (selectedCard)
    {
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);
        Vec2 finalPos = selectedCard->getPosition();

        bool inDropArea =
            abs(finalPos.x - center.x) <= 500 &&
            abs(finalPos.y - center.y) <= 300;

        Sprite* cardToHandle = selectedCard;
        selectedCard = nullptr;  // 先清除选中状态

        if (inDropArea) {
            // 在出牌区域内，执行移除
            removeCard(cardToHandle);
        }
        else {
            // 不在出牌区域，回到原位
            Vec2 originalPos = cardOriginalPositions[cardToHandle];

            // 使用缓动动画回到原位
            cardToHandle->runAction(Sequence::create(
                EaseBackOut::create(MoveTo::create(0.5f, originalPos)),
                CallFunc::create([cardToHandle]() {
                    cardToHandle->setColor(Color3B::WHITE);
                    cardToHandle->setOpacity(255);
                    }),
                nullptr
            ));

            // 如果鼠标仍在卡牌上，恢复放大效果
            if (hoveredCard == cardToHandle) {
                scaleSprite(cardToHandle, 1.5f);
            }
        }
    }
}