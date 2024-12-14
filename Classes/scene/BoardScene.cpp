/****************************************************************
* ����ҳ���ʵ��
* Author: Lee
 ****************************************************************/

#include "BoardScene.h"
#include "MainScene.h"
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

    //// ������ӿ��Ƶİ�ť(��ʱ���ڲ���)
    //auto addButton = MenuItemImage::create("button/addcard.png", "button/addcard.png", CC_CALLBACK_0(BoardScene::addNewCard, this));
    //addButton->setPosition(Vec2(1800, 400));
   
    // �������ذ�ť
    auto cancel = MenuItemImage::create("button/cancel.png", "button/cancelSelected.png", CC_CALLBACK_1(BoardScene::cancelCallback, this));
    cancel->setPosition(Vec2(1940,20));
    auto menu = Menu::create(cancel, nullptr);
    // ��Ӳ˵�
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
    
    // �����ƶ�������
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = CC_CALLBACK_1(BoardScene::onMouseMove, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    hoveredCard = nullptr;


    // ���������������
    createDropArea();

    // ����update,��֡ˢ��
    //this->scheduleUpdate();
    initPlayers();
    createPlayerUI();
    updatePlayerUI();
    initDecks();  // ��ʼ���ƶ�

    return true;
}

//�������˵�
void BoardScene::cancelCallback(Ref* pSender)
{
    // ���ص����Ч
    audioPlayer("Music/ClickSoundEffect.mp3", false);
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene()));
}

// ���������������
void BoardScene::createDropArea() 
{
    dropArea = DrawNode::create();

    // ��ȡ��Ļ���ĵ�
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 center = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

    // ���ư�͸���ľ�������
    Vec2 vertices[] = 
    {
        Vec2(center.x - PUTOUT_CARD_REGION_HALF_X, center.y - PUTOUT_CARD_REGION_HALF_Y),
        Vec2(center.x + PUTOUT_CARD_REGION_HALF_X, center.y - PUTOUT_CARD_REGION_HALF_Y),
        Vec2(center.x + PUTOUT_CARD_REGION_HALF_X, center.y + PUTOUT_CARD_REGION_HALF_Y),
        Vec2(center.x - PUTOUT_CARD_REGION_HALF_X, center.y + PUTOUT_CARD_REGION_HALF_Y)
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
        abs(spritePos.x - center.x) <= PUTOUT_CARD_REGION_HALF_X &&
        abs(spritePos.y - center.y) <= PUTOUT_CARD_REGION_HALF_Y;

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


//�ӳ��������Ƴ�һ�ſ���
void BoardScene::removeCard(Sprite* sprite) {
    if (!sprite) return;

    if (sprite == hoveredCard) {
        hoveredCard = nullptr;
    }

    // �������Ƴ�
    auto iter = find(dragCards.begin(), dragCards.end(), sprite);
    if (iter != dragCards.end()) {
        // ��ȡ���Ƴ����Ƶ�����λ��
        size_t removedIndex = distance(dragCards.begin(), iter);

        dragCards.erase(iter);
        cardOriginalPositions.erase(sprite);
        playedCards.push_back(sprite);
        updatePlayedCardsPosition();

        // ������Ч
        audioPlayer("music/putcard.mp3", false);

        // ����ʣ�࿨�Ƶ�λ��
        for (size_t i = removedIndex; i < dragCards.size(); i++) {
            Sprite* card = dragCards[i];
            // ������λ��
            Vec2 newPos(CARD_REGION_X + i * (card->getContentSize().width + 30), CARD_REGION_Y);

            // ���´洢��ԭʼλ��
            cardOriginalPositions[card] = newPos;

            // ����ƶ�����
            card->runAction(Sequence::create(
                EaseInOut::create(MoveTo::create(0.3f, newPos), 2.0f),
                EaseElasticOut::create(ScaleTo::create(0.2f, 1.0f)),
                nullptr
            ));
        }

        
    }
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
            abs(finalPos.x - center.x) <= PUTOUT_CARD_REGION_HALF_X &&
            abs(finalPos.y - center.y) <= PUTOUT_CARD_REGION_HALF_Y;

        Sprite* cardToHandle = selectedCard;
        selectedCard = nullptr;  // �����ѡ��״̬

        if (inDropArea) {
            // �ڳ��������ڣ�ִ���Ƴ�
           // removeCard(cardToHandle);
            Player* currentPlayer = isPlayer1Turn ? player1 : player2;

            // ������Ҫ��ӻ�ȡ���Ʒ��õ��߼�
            int cardCost = 1; // ʾ������

            if (currentPlayer->money >= cardCost) {
                currentPlayer->money -= cardCost;
                removeCard(cardToHandle);
                updatePlayerUI();
            }
            else 
            {
                // ���ò��㣬�ص�ԭλ
                Vec2 originalPos = cardOriginalPositions[cardToHandle];
                cardToHandle->runAction(EaseBackOut::create(MoveTo::create(0.5f, originalPos)));
            }
        }
        else 
        {
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

// ��ʼ�����
void BoardScene::initPlayers() 
{
    player1 = new Player("Player1");
    player2 = new Player("Player2");
    isPlayer1Turn = true;  // Ĭ�����1����

    // ��ʼ����ҿ���
    
    // ... ��ʼ������(������)
    //��� player.h��player.cpp
    player1->setPlayerCards();
    player2->setPlayerCards();
}

// ���������ϢUI
void BoardScene::createPlayerUI() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // �������������Ϣ��ʾ
    player1Health = Label::createWithTTF("HP: 30", "fonts/arial.ttf", 24);
    player1Health->setPosition(Vec2(800, 220));
    this->addChild(player1Health);
    // ������ߣ�ͬʱ������ɫ�ʹ�ϸ��
    player1Health->enableOutline(Color4B::BLACK, 2);         // ��ɫ��ߣ���ϸΪ2

    player1Mana = Label::createWithTTF("Mana: 1/1", "fonts/arial.ttf", 24);
    player1Mana->setPosition(Vec2(800, 280));
    this->addChild(player1Mana);
    // ������ߣ�ͬʱ������ɫ�ʹ�ϸ��
    player1Mana->enableOutline(Color4B::BLACK, 2);         // ��ɫ��ߣ���ϸΪ2

    // �����Է������Ϣ��ʾ
    player2Health = Label::createWithTTF("HP: 30", "fonts/arial.ttf", 24);
    player2Health->setPosition(Vec2(800, visibleSize.height - 220));
    this->addChild(player2Health);
    player2Health->enableOutline(Color4B::BLACK, 2);         // ��ɫ��ߣ���ϸΪ2

    player2Mana = Label::createWithTTF("Mana: 1/1", "fonts/arial.ttf", 24);
    player2Mana->setPosition(Vec2(800, visibleSize.height - 280));
    this->addChild(player2Mana);
    player2Mana->enableOutline(Color4B::BLACK, 2);         // ��ɫ��ߣ���ϸΪ2

    // �غ�ָʾ��
    turnIndicator = Label::createWithTTF("Your Turn", "fonts/arial.ttf", 32);
    turnIndicator->setPosition(Vec2(1880, 600));
    this->addChild(turnIndicator);
    // ������ߣ�ͬʱ������ɫ�ʹ�ϸ��
    turnIndicator->enableOutline(Color4B::BLACK, 2);         // ��ɫ��ߣ���ϸΪ2

    // ��ӻغϽ�����ť
    auto endTurnBtn = MenuItemImage::create(
        "button/endturn.png",
        "button/endturnSelected.png",
        CC_CALLBACK_0(BoardScene::switchTurn, this)
    );
    endTurnBtn->setPosition(Vec2(1880, 700));
    auto menu = Menu::create(endTurnBtn, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
}

// �������UI
void BoardScene::updatePlayerUI() {
    // ��������ֵ��ʾ
    player1Health->setString("HP: " + std::to_string(player1->health));
    player2Health->setString("HP: " + std::to_string(player2->health));

    // ���·���ֵ��ʾ
    player1Mana->setString("Mana: " + std::to_string(player1->money) + "/" +
        std::to_string(player1->maxmoney));
    player2Mana->setString("Mana: " + std::to_string(player2->money) + "/" +
        std::to_string(player2->maxmoney));

    // ���»غ�ָʾ
    turnIndicator->setString(isPlayer1Turn ? "Your Turn" : "Opponent's Turn");
}

// �л��غ�
void BoardScene::switchTurn() 
{
    isPlayer1Turn = !isPlayer1Turn;
    // ���ص����Ч
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
    // �л��غ�ʱ���߼�
    Player* currentPlayer = isPlayer1Turn ? player1 : player2;
    currentPlayer->maxmoney = std::min(currentPlayer->maxmoney + 1, 10);
    currentPlayer->money = currentPlayer->maxmoney;

    // ����UI
    updatePlayerUI();

    // ������ӻغ��л�����
    turnIndicator->runAction(Sequence::create(
        FadeOut::create(0.2f),
        CallFunc::create([this]() {
            updatePlayerUI();
            }),
        FadeIn::create(0.2f),
        nullptr
    ));
    // �غϿ�ʼʱ��һ����
    drawCard();
}

// ��ʼ���ƶ�
void BoardScene::initDecks() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // �����ƶ������ڵ�
    deckNode1 = Node::create();
    deckNode2 = Node::create();
    this->addChild(deckNode1);
    this->addChild(deckNode2);

    // �����ƶ�λ��
    deckNode1->setPosition(Vec2(1950, 500));
    deckNode2->setPosition(Vec2(1950, 900));

    // ��ʾ�����ƶ�
    for (int i = 0; i < 30; i++) 
    {
        // �������1���ƶ�
        player1->playerCards.at(i)->setPosition(Vec2(-i * 2.0f, -i * 2.0f));
        deckNode1->addChild(player1->playerCards.at(i));
        // �������2���ƶ�
        player2->playerCards.at(i)->setPosition(Vec2(-i * 2.0f, -i * 2.0f));
        deckNode2->addChild(player2->playerCards.at(i));
    }
}

// �ӿ����ƶ��г�һ����
void BoardScene::drawCard() {
    if (isPlayer1Turn && !player1->playerCards.empty()) {
        // ���ƶѳ�һ����
        auto card = player1->playerCards.back();
        player1->playerCards.pop_back();

        // �����µĿɲ������ƣ�����card������
        auto texture = card->getTexture();
        auto newCard = Sprite::createWithTexture(texture);

        // �����¿��Ƶĳ�ʼλ�ã����ƶ�λ�ÿ�ʼ��
        newCard->setPosition(Vec2(1950, 500)); // �ƶ�λ��
        newCard->setScale(0.5f); // ��ʼ��С�ĳߴ�

        // ����Ŀ��λ��
        Vec2 originalPos(CARD_REGION_X + dragCards.size() * (newCard->getContentSize().width + 30), CARD_REGION_Y);

        // ��ӵ����������򲢼�¼λ��
        this->addChild(newCard);
        cardOriginalPositions[newCard] = originalPos;
        dragCards.push_back(newCard);

        // ���ƶ�������
        newCard->runAction(Sequence::create(
            // 1. ����΢�ϸ�
            EaseOut::create(MoveBy::create(0.2f, Vec2(0, 50)), 2.0f),
            // 2. �ƶ���Ŀ��λ�ò��Ŵ�
            Spawn::create(
                EaseInOut::create(MoveTo::create(0.5f, originalPos), 2.0f),
                EaseInOut::create(ScaleTo::create(0.5f, 1.0f), 2.0f),
                RotateBy::create(0.5f, 360), // ��תһȦ
                nullptr
            ),
            // 3. �����΢����Ч��
            EaseElasticOut::create(ScaleTo::create(0.3f, 1.0f)),
            nullptr
        ));

        // ���ų�����Ч
        audioPlayer("Music/drawcard.mp3", false);

        // �����ƶ���ʾ
        deckNode1->removeChild(card);
    }
    else if (!player2->playerCards.empty()) {
        // �ڶ�����ҵĳ����߼�������ɣ�
    }
}

// ���³����Ѵ�����Ƶ���ʾ
void BoardScene::updatePlayedCardsPosition() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    float cardWidth = 100;  // ���迨�ƿ��Ϊ100
    float spacing = 20;     // ���Ƽ��

    // ������ʼλ�ã�ʹ���ƾ�����ʾ��
    float startX = center.x - (playedCards.size() * (cardWidth + spacing) - spacing) / 2;

    // ����ÿ���Ѵ�����Ƶ�λ��
    for (size_t i = 0; i < playedCards.size(); i++) {
        Sprite* card = playedCards[i];
        Vec2 targetPos = Vec2(startX + i * (cardWidth + spacing), center.y);

        // ʹ�ö����ƶ���Ŀ��λ��
        card->runAction(EaseBackOut::create(MoveTo::create(0.3f, targetPos)));
    }
}