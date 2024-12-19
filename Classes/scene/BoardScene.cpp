// BoardScene.cpp 

#include "BoardScene.h"
#include "MainScene.h"
#include "Player.h"
#include "cocos2d.h"
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"

#include "Common-cpp/inc/ValueObject.h"
#include "Common-cpp/inc/Helpers/ValueToObject.h"
#include "Photon-cpp/inc/Enums/ReceiverGroup.h"
#include "Common-cpp/inc/Containers/Hashtable.h"
#include "players/GameData.h"

#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <sstream>

// ʹ�� cocos2d �����ռ�
USING_NS_CC;

// ʹ�������ռ������ Photon SDK ����������
namespace EG = ExitGames::Common;

// ���� cardNumber Ϊ int
typedef int CardNumber;

// ������ұ������
typedef int PlayerNumber;

// ��������
Scene* BoardScene::createScene() {
    return BoardScene::create();
}

// ��ӡ���ش�����Ϣ
static void problemLoading(const char* filename) {
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in BoardScene.cpp\n");
}

// ��ʼ��
bool BoardScene::init() {
    if (!Scene::init()) {
        return false;
    }

    // ��ȡȫ�� PhotonLib ʵ��
    photonLib = PhotonLib::getInstance();
    if (!photonLib) {
        CCLOG("Failed to get PhotonLib instance.");
        CocosUIListener::getInstance()->writeString(EG::JString(L"Failed to get PhotonLib instance."));
        return false;
    }

    // ��ȡȫ�� CocosUIListener ʵ��
    cocosUIListener = CocosUIListener::getInstance();
    if (!cocosUIListener) {
        CCLOG("Failed to get CocosUIListener instance.");
        return false;
    }

    // ����һ������UI��Layer�����MainScene���Ѿ��У�����ʡ�ԣ�
    auto uiLayer = Layer::create();
    this->addChild(uiLayer, 100); // ȷ��UI�������ϲ�

    // �� CocosUIListener ���ӵ� UI Layer ��
    cocosUIListener->attachToLayer(uiLayer, Vec2(100, 100)); // ������Ҫ����λ��

    // ���� PhotonLib ���Զ����¼��ص�
    photonLib->setCustomEventCallback(CC_CALLBACK_2(BoardScene::onPhotonEvent, this));

    // ��ʼ�����
    initPlayers();

    // ���������ϢUI
    createPlayerUI();

    // ��ȡ��Ļ�ߴ��ԭ��
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ���ӱ���ͼƬ
    auto title_sprite = Sprite::create("board.png");
    if (title_sprite == nullptr) {
        problemLoading("board.png");
        cocosUIListener->writeString(EG::JString(L"Failed to load board.png"));
        return false;
    }
    else {
        // ����ͼƬ�ĳߴ�Ϊ���ڵĳߴ�
        title_sprite->setContentSize(visibleSize);
        // ��ͼƬ��λ�õ���Ϊ���ڵ�����
        title_sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(title_sprite, 0);
    }

    // �������ذ�ť
    auto cancel = MenuItemImage::create(
        "button/cancel.png",
        "button/cancelSelected.png",
        CC_CALLBACK_1(BoardScene::cancelCallback, this)
    );
    if (cancel == nullptr || cancel->getContentSize().width <= 0 || cancel->getContentSize().height <= 0)
    {
        problemLoading("'cancel.png' and 'cancelSelected.png'");
    }
    else
    {
        // ʹ�õ�һ���ṩ�Ĳ���
        cancel->setPosition(Vec2(1940, 20)); // ���һ��ʵ��һ��
    }
    auto menu = Menu::create(cancel, nullptr);

    // ���Ӳ˵�
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // ��������������
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(BoardScene::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(BoardScene::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(BoardScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    selectedCard = nullptr;

    // ������������
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = CC_CALLBACK_1(BoardScene::onMouseMove, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    hoveredCard = nullptr;

    // ���������������
    createDropArea();

    // ���� update �����ĵ���
    this->scheduleUpdate();

    distributeInitialHands(); // ͨ���¼��ַ�����

    // ������һ����ҵĻغ�
    if (localPlayerNumber == 1)
    {
        sendTurnStartEvent(); // ������һ����ҵĻغϿ�ʼ
    }
    return true;
}

// �������˵�
void BoardScene::cancelCallback(Ref* pSender)
{
    // ���ص����Ч
    audioPlayer("Music/ClickSoundEffect.mp3", false);
    // �뿪����
    photonLib->leaveRoom();
    cocosUIListener->writeString(EG::JString(L"Leaving room and returning to main menu."));
    // �л������˵�
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene(), Color3B::WHITE));
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
    this->addChild(dropArea, 1);
}

// ���� checkDropArea ����
void BoardScene::checkDropArea() {
    if (!selectedCard)
        return;

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2 + Director::getInstance()->getVisibleOrigin().x,
        visibleSize.height / 2 + Director::getInstance()->getVisibleOrigin().y);
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

            // ���ƴֱ߿��������ñ߿����Ϊ5����
            Vec2 vertices[] = {
                Vec2(0, 0),
                Vec2(cardSize.width, 0),
                Vec2(cardSize.width, cardSize.height),
                Vec2(0, cardSize.height)
            };

            outline->drawPolygon(vertices, 4,
                Color4F(1, 0, 0, 0.0f),     // �����ɫ��͸����
                5.0f,                        // �߿����
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

// ����ƶ����
void BoardScene::onMouseMove(Event* event) {
    EventMouse* mouseEvent = dynamic_cast<EventMouse*>(event);
    Vec2 mousePos = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());

    // �������Ƿ���ͣ��ĳ��������
    Sprite* newHoveredSprite = nullptr;

    // ʹ�� localPlayerCards
    for (auto it = localPlayerCards.rbegin(); it != localPlayerCards.rend(); ++it) {
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

// ƽ�����ž���
void BoardScene::scaleSprite(Sprite* sprite, float scale) {
    // ʹ�ö���ʵ��ƽ��������Ч��
    sprite->stopAllActions();
    sprite->runAction(ScaleTo::create(0.1f, scale));
}

// ������ʼ
bool BoardScene::onTouchBegan(Touch* touch, Event* event)
{
    // ֻ���ڱ�����ҵĻغϲ���������
    if (!isLocalPlayerTurn) {
        return false;
    }

    Vec2 touchLocation = touch->getLocation();

    // �Ӻ���ǰ��飨ʹ���ϲ�ľ���������Ӧ��
    for (auto it = localPlayerCards.rbegin(); it != localPlayerCards.rend(); ++it) {
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

// �����ƶ�
void BoardScene::onTouchMoved(Touch* touch, Event* event)
{
    if (selectedCard)
    {
        selectedCard->setPosition(selectedCard->getPosition() + touch->getDelta());
    }
}

// ��������
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
            players::Player* currentPlayer = (currentPlayerNumber == 1) ? player1 : player2;
            PlayerNumber playerNumber = currentPlayerNumber; // ��ǰ�غ���ұ��

            // ��ȡ���Ʒ��õ��߼������迨�Ʒ��ô洢�ڿ��Ƶ� Tag �����������У�
            int cardCost = getCardCost(cardToHandle); // ��Ҫʵ�� getCardCost ����

            if (currentPlayer->getMoney() >= cardCost) {
                // �۳�����ֵ
                currentPlayer->setMoney(currentPlayer->getMoney() - cardCost);

                // ��ȡ���Ʊ��
                CardNumber cardNumber = cardToHandle->getTag();

                // ���ʹ����¼������� playerNumber �� cardNumber
                sendPlayCardEvent(playerNumber, cardNumber);

                // �Ƴ����Ʋ����� UI
                removeCard(cardToHandle);
                updatePlayerUI();

                // ���Ŵ�����Ч
                audioPlayer("Music/playcard.mp3", false);
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

// ��ȡ���Ʒ��õķ�����ʾ��ʵ�֣�
int BoardScene::getCardCost(Sprite* card) {
    // ���迨�Ʒ��ô洢�� Tag �У���ͨ��������ʽ��ȡ
    // ����ʹ�� Tag ��Ϊʾ��
    return card->getTag(); // ��Ҫ����ʵ���������
}

void BoardScene::initPlayers()
{
    localPlayerNumber = photonLib->getLocalPlayerNumber();
    currentPlayerNumber = 1;
    player1 = GameData::getInstance().getPlayer1();
    player2 = GameData::getInstance().getPlayer2();

    // ���� currentPlayerNumber �� localPlayerNumber ���� isLocalPlayerTurn
    isLocalPlayerTurn = (currentPlayerNumber == localPlayerNumber);
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
    turnIndicator->setPosition(Vec2(1880, 600)); // ���һ��ʵ��һ��
    this->addChild(turnIndicator);
    // ������ߣ�ͬʱ������ɫ�ʹ�ϸ��
    turnIndicator->enableOutline(Color4B::BLACK, 2);         // ��ɫ��ߣ���ϸΪ2

    // ���ӻغϽ�����ť����������ҿ��Կ�����
    auto endTurnBtn = MenuItemImage::create(
        "button/endturn.png",
        "button/endturnSelected.png",
        [this](Ref* sender) {
            this->switchTurn();
        }
    );
    endTurnBtn->setPosition(Vec2(1880, 700)); // ���һ��ʵ��һ��
    auto menu = Menu::create(endTurnBtn, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
}

// �������UI
void BoardScene::updatePlayerUI() {
    // ��������ֵ��ʾ
    player1Health->setString("HP: " + std::to_string(player1->getHealth()));
    player2Health->setString("HP: " + std::to_string(player2->getHealth()));

    // ���·���ֵ��ʾ
    player1Mana->setString("Mana: " + std::to_string(player1->getMoney()) + "/" +
        std::to_string(player1->getMaxMoney()));
    player2Mana->setString("Mana: " + std::to_string(player2->getMoney()) + "/" +
        std::to_string(player2->getMaxMoney()));

    // ���»غ�ָʾ
    if (isLocalPlayerTurn) {
        turnIndicator->setString("Your Turn");
    }
    else {
        turnIndicator->setString("Opponent's Turn");
    }
}

// �л��غ�
void BoardScene::switchTurn()
{
    if (!isLocalPlayerTurn) {
        CCLOG("It's not your turn.");
        cocosUIListener->writeString(EG::JString(L"It's not your turn."));
        return;
    }

    // ������һ����ұ�ţ���2����Ϸ�У���ұ��Ϊ1��2��
    int nextPlayerNumber = (currentPlayerNumber == 1) ? 2 : 1;

    // ���µ�ǰ��ұ��
    currentPlayerNumber = nextPlayerNumber;

    // �����Ƿ�Ϊ������ҵĻغ�
    isLocalPlayerTurn = (currentPlayerNumber == localPlayerNumber);

    // ���� TURN_START �¼�
    sendTurnStartEvent();

    // ���� UI
    updatePlayerUI();

    CCLOG("Switched turn to playerNumber: %d", currentPlayerNumber);
    cocosUIListener->writeString(EG::JString(L"Switched turn."));
}

// ���ͻغϿ�ʼ�¼�
void BoardScene::sendTurnStartEvent() {
    EG::Hashtable eventContent;
    // ���ݵ�ǰ��ұ��
    PlayerNumber currentPlayerNum = currentPlayerNumber;
    eventContent.put(static_cast<unsigned char>(0), EG::Helpers::ValueToObject<EG::Object>::get(currentPlayerNum));

    photonLib->raiseCustomEvent(eventContent, TURN_START, ExitGames::Lite::ReceiverGroup::OTHERS);
    CCLOG("Sent TURN_START event for playerNumber: %d", currentPlayerNum);
    cocosUIListener->writeString(EG::JString(L"Sent TURN_START event."));
}

// �ӿ����Ƴ������ӵ�ս��
void BoardScene::removeCard(Sprite* sprite) {
    if (!sprite) return;

    if (sprite == hoveredCard) {
        hoveredCard = nullptr;
    }

    // �� localPlayerCards �Ƴ�
    auto iter = std::find(localPlayerCards.begin(), localPlayerCards.end(), sprite);
    if (iter != localPlayerCards.end()) {
        // ��ȡ���Ƴ����Ƶ�����λ��
        size_t removedIndex = std::distance(localPlayerCards.begin(), iter);

        localPlayerCards.erase(iter);
        cardOriginalPositions.erase(sprite);
        playedCards.push_back(sprite);
        updatePlayedCardsPosition();

        // ����ҵ��������Ƴ����Ʊ��
        CardNumber cardNumber = sprite->getTag();
        players::Player* currentPlayer = (currentPlayerNumber == 1) ? player1 : player2;
        currentPlayer->removeCardFromHand(cardNumber);

        // ������Ч
        audioPlayer("Music/putcard.mp3", false);

        // ����ʣ�࿨�Ƶ�λ��
        for (size_t i = removedIndex; i < localPlayerCards.size(); i++) {
            Sprite* card = localPlayerCards[i];
            // ������λ��
            Vec2 newPos(CARD_REGION_X + i * (card->getContentSize().width + 30), CARD_REGION_Y);

            // ���´洢��ԭʼλ��
            cardOriginalPositions[card] = newPos;

            // �����ƶ�����
            card->runAction(Sequence::create(
                EaseInOut::create(MoveTo::create(0.3f, newPos), 2.0f),
                EaseElasticOut::create(ScaleTo::create(0.2f, 1.0f)),
                nullptr
            ));
        }
    }
}

// ���³����Ѵ�����Ƶ���ʾ
void BoardScene::updatePlayedCardsPosition() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    float cardWidth = 100;  // ���迨�ƿ���Ϊ100
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

// ���ʹ����¼�
void BoardScene::sendPlayCardEvent(PlayerNumber playerNumber, CardNumber cardNumber) {
    EG::Hashtable eventContent;
    // ʹ�ò�ͬ�� key ������ playerNumber �� cardNumber
    eventContent.put(static_cast<unsigned char>(0), EG::Helpers::ValueToObject<EG::Object>::get(playerNumber));
    eventContent.put(static_cast<unsigned char>(1), EG::Helpers::ValueToObject<EG::Object>::get(cardNumber));

    photonLib->raiseCustomEvent(eventContent, PLAY_CARD, ExitGames::Lite::ReceiverGroup::OTHERS);
    CCLOG("Sent PLAY_CARD event with playerNumber: %d, cardNumber: %d", playerNumber, cardNumber);
    cocosUIListener->writeString(EG::JString(L"Sent PLAY_CARD event with playerNumber: ") +
        EG::JString(std::to_wstring(playerNumber).c_str()) +
        EG::JString(L", cardNumber: ") +
        EG::JString(std::to_wstring(cardNumber).c_str()));
}

// ���� Photon �Զ����¼�
void BoardScene::onPhotonEvent(int eventCode, const EG::Hashtable& parameters) {
    switch (eventCode) {
        case PLAY_CARD:
            handlePlayCard(parameters);
            break;
        case TURN_START:
            handleTurnStart(parameters);
            break;
        default:
            CCLOG("Received unknown eventCode: %d", eventCode);
            cocosUIListener->writeString(EG::JString(L"Received unknown eventCode: ") +
                EG::JString(std::to_wstring(eventCode).c_str()));
            break;
    }
}

// ���������¼�
void BoardScene::handlePlayCard(const EG::Hashtable& parameters) {
    int playerNumber = 0;
    int cardNumber = 0;

    // ��ȡ playerNumber
    const EG::Object* objPlayerNumber = parameters.getValue(static_cast<unsigned char>(0));
    if (objPlayerNumber) {
        const EG::ValueObject<int>* voPlayerNumber = dynamic_cast<const EG::ValueObject<int>*>(objPlayerNumber);
        if (voPlayerNumber) {
            playerNumber = voPlayerNumber->getDataCopy();
            CCLOG("Received PLAY_CARD for playerNumber: %d", playerNumber);
        }
        else {
            CCLOG("Failed to cast playerNumber in PLAY_CARD event.");
            cocosUIListener->writeString(EG::JString(L"Failed to cast playerNumber in PLAY_CARD event."));
            return;
        }
    }
    else {
        CCLOG("playerNumber not found in PLAY_CARD event.");
        cocosUIListener->writeString(EG::JString(L"playerNumber not found in PLAY_CARD event."));
        return;
    }

    // ��ȡ cardNumber
    const EG::Object* objCardNumber = parameters.getValue(static_cast<unsigned char>(1));
    if (objCardNumber) {
        const EG::ValueObject<int>* voCardNumber = dynamic_cast<const EG::ValueObject<int>*>(objCardNumber);
        if (voCardNumber) {
            cardNumber = voCardNumber->getDataCopy();
            CCLOG("Received PLAY_CARD with cardNumber: %d for playerNumber: %d", cardNumber, playerNumber);
        }
        else {
            CCLOG("Failed to cast cardNumber in PLAY_CARD event.");
            cocosUIListener->writeString(EG::JString(L"Failed to cast cardNumber in PLAY_CARD event."));
            return;
        }
    }
    else {
        CCLOG("cardNumber not found in PLAY_CARD event.");
        cocosUIListener->writeString(EG::JString(L"cardNumber not found in PLAY_CARD event."));
        return;
    }

    // ȷ��Ŀ�����
    players::Player* targetPlayer = nullptr;
    if (playerNumber == 1) {
        targetPlayer = player1;
    }
    else if (playerNumber == 2) {
        targetPlayer = player2;
    }
    else {
        CCLOG("Invalid playerNumber in PLAY_CARD event: %d", playerNumber);
        return;
    }

    // ������������ҵĴ����¼�
    if ((playerNumber == 1 && localPlayerNumber == 1) ||
        (playerNumber == 2 && localPlayerNumber == 2)) {
        // �ҵ���Ӧ�Ŀ���
        Sprite* card = findCardByID(cardNumber);

        if (card) {
            // �����ƴ������Ƴ������ӵ�ս��
            removeCard(card);
            updatePlayerUI();
        }
    }
    else {
        // ����Ƕ�����ҵĴ����¼������¶�����ҵ�״̬�����������޸��κξ���
        CCLOG("Opponent player %d played cardNumber: %d. No sprite to handle.", playerNumber, cardNumber);
        cocosUIListener->writeString(EG::JString(L"Opponent played a card."));
        targetPlayer->removeCardFromHand(cardNumber);
    }
}

// �����غϿ�ʼ�¼�
void BoardScene::handleTurnStart(const EG::Hashtable& parameters) {
    int receivedPlayerNumber = 0;

    // ��ȡ receivedPlayerNumber
    const EG::Object* objPlayerNumber = parameters.getValue(static_cast<unsigned char>(0));
    if (objPlayerNumber && objPlayerNumber->getType() == EG::TypeCode::INTEGER) {
        const EG::ValueObject<int>* voPlayerNumber = static_cast<const EG::ValueObject<int>*>(objPlayerNumber);
        if (voPlayerNumber) {
            receivedPlayerNumber = voPlayerNumber->getDataCopy();
            CCLOG("Received TURN_START for playerNumber: %d", receivedPlayerNumber);
        }
        else {
            CCLOG("Failed to cast playerNumber in TURN_START event.");
            cocosUIListener->writeString(EG::JString(L"Failed to cast playerNumber in TURN_START event."));
            return;
        }
    }
    else {
        CCLOG("playerNumber not found or incorrect TypeCode in TURN_START event.");
        cocosUIListener->writeString(EG::JString(L"playerNumber not found or incorrect TypeCode in TURN_START event."));
        return;
    }
    
    // ���µ�ǰ��ұ�źͱ��ػغ�״̬
    currentPlayerNumber = receivedPlayerNumber;
    isLocalPlayerTurn = (currentPlayerNumber == localPlayerNumber);

    // ����UI
    updatePlayerUI();

    // ����Ǳ�����ҵĻغϣ���һ�ſ���
    if (isLocalPlayerTurn) {
        players::Player* currentPlayer = (currentPlayerNumber == 1) ? player1 : player2;
        if (currentPlayer->hasCards()) {
            CardNumber cardNumber = currentPlayer->drawCard(); // ��ȡһ�ſ���
            if (cardNumber != -1) {
                addCardToLocalPlayer(cardNumber);
            }
        }
        else {
            endGame((currentPlayerNumber == 1) ? player2 : player1);
        }
    }

    cocosUIListener->writeString(EG::JString(L"Handled TURN_START event. Switched turn."));
}

// �ַ���ʼ����
void BoardScene::distributeInitialHands()
{
    // ȷ����ʼ��������������ұ��
    int initialHandSize = 0;
    if (localPlayerNumber == 1) {
        initialHandSize = 4; // ������ҳ�ȡ4����
    }
    else if (localPlayerNumber == 2) {
        initialHandSize = 3; // ������ҳ�ȡ3����
    }
    else {
        CCLOG("Invalid localPlayerNumber: %d", localPlayerNumber);
        cocosUIListener->writeString(EG::JString(L"Invalid localPlayerNumber."));
        return;
    }

    // Ϊ������ҳ�ȡ��ʼ����
    for (int i = 0; i < initialHandSize; ++i) {
        players::Player* localPlayer = (localPlayerNumber == 1) ? player1 : player2;
        CardNumber cardNumber = localPlayer->drawCard();
        if (cardNumber != -1) {
            addCardToLocalPlayer(cardNumber);
            CCLOG("Player %d drew initial cardNumber: %d", localPlayer->getPlayerNumber(), cardNumber);
            cocosUIListener->writeString(EG::JString(L"Player drew initial card."));
        }
        else {
            CCLOG("Player %d failed to draw initial card.", localPlayer->getPlayerNumber());
            cocosUIListener->writeString(EG::JString(L"Failed to draw initial card."));
        }
    }
}

// ���ӿ��Ƶ�������ң���������ʾ��
void BoardScene::addCardToLocalPlayer(CardNumber cardNumber) {
    players::Player* localPlayer = (localPlayerNumber == 1) ? player1 : player2;
    localPlayer->addCardToHand(cardNumber);

    // �������ƾ���
    auto newCard = Sprite::create("cardfortest.png"); // ʹ�ÿ�����������
    if (newCard) {
        newCard->setTag(cardNumber); // ʹ�� cardNumber ��Ϊ tag

        // ���ó�ʼλ�û��ڳ��� CARD_REGION_X �� CARD_REGION_Y
        Vec2 originalPos = Vec2(
            CARD_REGION_X + (localPlayer->getHand().size() - 1) * (newCard->getContentSize().width + 30),
            CARD_REGION_Y
        );

        // ���ӵ�����
        this->addChild(newCard);
        cardOriginalPositions[newCard] = originalPos;
        localPlayerCards.push_back(newCard);

        // ���ÿ���λ�ú�����
        newCard->setPosition(originalPos);
        newCard->setScale(1.0f);

        // ���ж���
        newCard->runAction(Sequence::create(
            EaseOut::create(MoveBy::create(0.2f, Vec2(0, 50)), 2.0f),
            Spawn::create(
                EaseInOut::create(MoveTo::create(0.5f, originalPos), 2.0f),
                EaseInOut::create(ScaleTo::create(0.5f, 1.0f), 2.0f),
                RotateBy::create(0.5f, 360), // ��תһȦ
                nullptr
            ),
            EaseElasticOut::create(ScaleTo::create(0.3f, 1.0f)),
            nullptr
        ));

        // ���ų�����Ч
        audioPlayer("Music/drawcard.mp3", false);

        // ����UI
        updatePlayerUI();

        // ��־
        cocosUIListener->writeString(EG::JString(L"Added initial card locally."));
    }
    else {
        CCLOG("Failed to create card sprite for cardNumber: %d", cardNumber);
        cocosUIListener->writeString(EG::JString(L"Failed to create card sprite for cardNumber: ") +
            EG::JString(std::to_wstring(cardNumber).c_str()));
    }
}


// �������������ݿ���ID���Ҿ���
Sprite* BoardScene::findCardByID(int cardNumber) {
    for (auto& cardSprite : localPlayerCards) {
        if (cardSprite->getTag() == cardNumber) {
            return cardSprite;
        }
    }

    CCLOG("Local Player's card with cardNumber %d not found.", cardNumber);
    return nullptr;
}


// ������Ϸ����
void BoardScene::endGame(players::Player* winner) {
    // ��ʾ��Ϸ��������
    std::string result = winner->getNickname() + " Wins!";
    auto resultLabel = Label::createWithTTF(result, "fonts/arial.ttf", 48);
    resultLabel->setPosition(Director::getInstance()->getVisibleSize() / 2);
    this->addChild(resultLabel, 10);

    // ֹͣ��Ϸ�е����в���
    // ���磬���ð�ť��ֹͣ��ʱ��
    cocosUIListener->writeString(EG::JString(L"Game Over: ") +
        EG::JString(result.c_str()));
}

// �����߼�
void BoardScene::update(float dt) {
    // ����PhotonLib
    if (photonLib) {
        photonLib->update();
    }
}

// ��������
BoardScene::~BoardScene()
{
    // �������ƾ���
    for (auto& card : localPlayerCards) {
        if (card) {
            card->removeFromParent();
        }
    }
    localPlayerCards.clear();

    for (auto& card : playedCards) {
        if (card) {
            card->removeFromParent();
        }
    }
    playedCards.clear();

    // ��������λ�ü�¼
    cardOriginalPositions.clear();

    player1 = nullptr;
    player2 = nullptr;
    CCLOG("BoardScene destroyed and all card references cleared.");
}
