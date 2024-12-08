/****************************************************************
* ����ҳ���ʵ��
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
        // ����ͼƬ�ĳߴ�Ϊ���ڵĳߴ�
        title_sprite->setContentSize(visibleSize);
        // ��ͼƬ��λ�õ���Ϊ���ڵ�����
        title_sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(title_sprite, 0);
    }

    // ������ӿ��Ƶİ�ť(��ʱ���ڲ���)
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

    // ��������������
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


    // ���������������
    createDropArea();

    // ����update,��֡ˢ��
    //this->scheduleUpdate();

    return true;
}

// ���������������
void BoardScene::createDropArea() 
{
    dropArea = DrawNode::create();

    // ��ȡ��Ļ���ĵ�
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 center = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

    // ���ư�͸���ľ�������500x200��
    Vec2 vertices[] = 
    {
        Vec2(center.x - 500, center.y - 300),
        Vec2(center.x + 500, center.y - 300),
        Vec2(center.x + 500, center.y + 300),
        Vec2(center.x - 500, center.y + 300)
    };

    Color4F fillColor(0.5f, 0.5f, 0.5f, 0.3f); // ��͸����ɫ
    Color4F borderColor(1.0f, 1.0f, 1.0f, 0.8f); // ��ɫ�߿�

    dropArea->drawPolygon(vertices, 4, fillColor, 2, borderColor);
    this->addChild(dropArea,1);
}

// ��鿨���Ƿ���Ͷ������
void BoardScene::checkDropArea() {
    if (!selectedCard)
        return;

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    Vec2 spritePos = selectedCard->getPosition();

    // ����Ƿ���Ͷ��������
    bool inDropArea =
        abs(spritePos.x - center.x) <= 500 &&
        abs(spritePos.y - center.y) <= 300;

    if (inDropArea)
    {
        // ֱ�����ÿ�����ɫ�����
        selectedCard->setColor(Color3B(255, 100, 100));  // ʹ�ý�ǳ�ĺ�ɫ

        // ����һ�����Ч��
        if (!selectedCard->getChildByName("outline"))
        {
            auto cardSize = selectedCard->getContentSize();
            auto outline = DrawNode::create();
            outline->setName("outline");

            // ���ƴֱ߿��������ñ߿���Ϊ5����
            Vec2 vertices[] = {
                Vec2(0, 0),
                Vec2(cardSize.width, 0),
                Vec2(cardSize.width, cardSize.height),
                Vec2(0, cardSize.height)
            };

            outline->drawPolygon(vertices, 4,
                Color4F(1, 0, 0, 0.0f),     // �����ɫ��͸����
                100.0f,                        // �߿���
                Color4F(1, 0, 0, 1.0f)      // �߿���ɫ����ɫ��
            );

            selectedCard->addChild(outline, 1);
        }
    }
    else
    {
        selectedCard->setColor(Color3B::WHITE);
        // �Ƴ����Ч��
        auto outline = selectedCard->getChildByName("outline");
        if (outline) {
            outline->removeFromParent();
        }
    }
}

//���һ���µĿ���
void BoardScene::addNewCard() 
{
    // �����¾���
    auto sprite = Sprite::create("cardfortest.png");
    
    Vec2 originalPos(Vec2(CARD_REGION_X + dragCards.size() * (sprite->getContentSize().width + 30), CARD_REGION_Y));
    sprite->setPosition(originalPos);

    //��������Ч
    audioPlayer("music/putcard.mp3", false);

    // ��¼ԭʼλ��
    cardOriginalPositions[sprite] = originalPos;

    // ��ӵ�������������
    this->addChild(sprite);
    dragCards.push_back(sprite);
    CCLOG("Current sprite count: %zu", dragCards.size());
}


//�ӳ��������Ƴ�һ�ſ���
void BoardScene::removeCard(Sprite* sprite) {
    if (!sprite) return;

    if (sprite == hoveredCard) {
        hoveredCard = nullptr;
    }

    // ��λ�ü�¼���Ƴ�
    cardOriginalPositions.erase(sprite);
    //��������Ч
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

// ����ƶ����
void BoardScene::onMouseMove(Event* event) {
    EventMouse* mouseEvent = dynamic_cast<EventMouse*>(event);
    Vec2 mousePos = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());

    // �������Ƿ���ͣ��ĳ��������
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

    // �����ͣ�ľ��鷢���仯
    if (hoveredCard != newHoveredSprite) {
        // �ָ�֮ǰ��ͣ����Ĵ�С
        if (hoveredCard && hoveredCard != selectedCard) {
            scaleSprite(hoveredCard, 1.0f);
        }

        // �Ŵ��µ���ͣ����
        if (newHoveredSprite && newHoveredSprite != selectedCard) {
            scaleSprite(newHoveredSprite, 1.5f); // �Ŵ�1.5��
        }

        hoveredCard = newHoveredSprite;
    }
}
void BoardScene::scaleSprite(Sprite* sprite, float scale) {
    // ʹ�ö���ʵ��ƽ��������Ч��
    sprite->runAction(ScaleTo::create(0.1f, scale));
}

// ��괥�����
bool BoardScene::onTouchBegan(Touch* touch, Event* event)
{
    Vec2 touchLocation = touch->getLocation();

    // �Ӻ���ǰ��飨ʹ���ϲ�ľ���������Ӧ��
    for (auto it = dragCards.rbegin(); it != dragCards.rend(); ++it) {
        Sprite* sprite = *it;
        Vec2 locationInNode = sprite->convertToNodeSpace(touchLocation);
        Size s = sprite->getContentSize();
        Rect rect = Rect(0, 0, s.width, s.height);

        if (rect.containsPoint(locationInNode)) {
            selectedCard = sprite;
            // ��ʼ�϶�ʱ�ָ�������С
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
        selectedCard = nullptr;  // �����ѡ��״̬

        if (inDropArea) {
            // �ڳ��������ڣ�ִ���Ƴ�
            removeCard(cardToHandle);
        }
        else {
            // ���ڳ������򣬻ص�ԭλ
            Vec2 originalPos = cardOriginalPositions[cardToHandle];

            // ʹ�û��������ص�ԭλ
            cardToHandle->runAction(Sequence::create(
                EaseBackOut::create(MoveTo::create(0.5f, originalPos)),
                CallFunc::create([cardToHandle]() {
                    cardToHandle->setColor(Color3B::WHITE);
                    cardToHandle->setOpacity(255);
                    }),
                nullptr
            ));

            // ���������ڿ����ϣ��ָ��Ŵ�Ч��
            if (hoveredCard == cardToHandle) {
                scaleSprite(cardToHandle, 1.5f);
            }
        }
    }
}