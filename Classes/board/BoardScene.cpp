#include "BoardScene.h"
#include <vector>

using namespace std;
USING_NS_CC;

// 卡片区域的起始位置坐标 注：需设置精灵锚点为左下角
#define CARD_REGION_X 100
#define CARD_REGION_Y 20


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

    createCards();
    
    return true;
}

bool BoardScene::createCards()
{
    for (int i = 0; i < 5; i++)
    {
        auto card = Sprite::create("cardfortest.png");
        if (card == nullptr)
        {
            problemLoading("cardfortest.png");
            return false;
        }

        card->setPosition(Vec2(CARD_REGION_X + i * 120, CARD_REGION_Y));
        card->setScale(1.0f);
        card->setTag(i);
        card->setAnchorPoint(Vec2(0, 0));
        cards.push_back(card);
        this->addChild(card, 0);

        // 创建鼠标事件监听器
        auto listener = EventListenerMouse::create();
        listener->onMouseDown = [this, card, i](Event* event) {
            onMouseDown(event, i);
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, cards.at(i));
    }

    return true;
}

bool BoardScene::onMouseDown(Event* event, int cardIndex)
{
    auto mouseEvent = static_cast<EventMouse*>(event);

    // 左键点击时的逻辑
    if (mouseEvent->getMouseButton() == cocos2d::EventMouse::MouseButton::BUTTON_LEFT)
    {
        //auto card = cards[cardIndex]; // 获取当前被点击的卡片
        auto card = cards.at(cardIndex); // 获取当前被点击的卡片
        if (card == nullptr)
        {
            problemLoading("cardfortest.png");
            return false;
        }

        // 如果当前卡片已经被选中，取消选中
        if (selectedCard == card)
        {
            selectedCard->setScale(1.0f); // 恢复原大小
            selectedCard = nullptr; // 取消选中
        }
        else
        {
            // 如果已有卡片被选中，取消选中
            if (selectedCard)
                selectedCard->setScale(1.0f); // 恢复原大小

            // 选中当前点击的卡片
            card->setScale(1.5f); // 放大选中的牌
            selectedCard = card;  // 记录选中的牌
        }
    }
}