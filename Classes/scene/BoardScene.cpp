// BoardScene.cpp 
#include "BoardScene.h"
#include "MainScene.h"
#include "players/player.h"
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

    // ��ʼ��ս����������
    localplayedCards.clear();
    oppentplayedCards.clear();

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

    // ��ȡ��Ļ�ߴ��ԭ��
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ��ӱ���ͼƬ
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
        cancel->setPosition(Vec2(1940, 80)); // ���һ��ʵ��һ��
    }
    auto menu = Menu::create(cancel, nullptr);

    // ��Ӳ˵�
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

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

    // ��ʼ�����
    initPlayers();

    // ���������ϢUI
    createPlayerUI();

    this->scheduleUpdate();

    distributeInitialHands(); // ͨ���¼��ַ�����

    if (localPlayerNumber == 1)
    {
        sendTurnStartEvent();
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
                5.0f,                        // �߿���
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
    cardSprite* newHoveredSprite = nullptr;

    for (auto it = localPlayerCards.rbegin(); it != localPlayerCards.rend(); ++it) {
        cardSprite* sprite = *it;
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
void BoardScene::scaleSprite(cardSprite* sprite, float scale) {
    // ʹ�ö���ʵ��ƽ��������Ч��
    sprite->runAction(ScaleTo::create(0.1f, scale));
}

// ��괥�����
bool BoardScene::onTouchBegan(Touch* touch, Event* event)
{
    if (!isLocalPlayerTurn) {
        return false;
    }

    Vec2 touchLocation = touch->getLocation();

    // �Ӻ���ǰ��飨ʹ���ϲ�ľ���������Ӧ��
    for (auto it = localPlayerCards.rbegin(); it != localPlayerCards.rend(); ++it) {
        cardSprite* sprite = *it;
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

        cardSprite* cardToHandle = selectedCard;
        selectedCard = nullptr;  // �����ѡ��״̬

        if (inDropArea) {
            // �ڳ��������ڣ�ִ���Ƴ�
            players::Player* currentPlayer = (currentPlayerNumber == 1) ? player1 : player2;
            if (currentPlayer) {
                // ��ȡ���Ʒ���
                // int cardNumber = cardToHandle->getTag();
                int cardCost = getCardCost(cardToHandle->card);
                if (currentPlayer->getMoney() >= cardCost) { // ʹ�� getter ����
                    // �۳�����ֵ
                    currentPlayer->setMoney(currentPlayer->getMoney() - cardCost);
                    CCLOG("Player %d mana reduced to %d", localPlayerNumber, currentPlayer->getMoney());

                    // ����UI
                    updatePlayerUI();

                    // ���� PLAY_CARD �¼�
                    /////////////////// need to be editted!!!
                    sendPlayCardEvent(localPlayerNumber, cardToHandle->card->dbfId);

                    // ��ʾ������ս����
                    /////////////////// need to be editted!!!
                    addCardToBattlefield(localPlayerNumber, cardToHandle->card->dbfId);

                    // ���������Ƴ�����
                    removeCard(cardToHandle);
                }
                else
                {
                    // ���ò��㣬�ص�ԭλ
                    Vec2 originalPos = cardOriginalPositions[cardToHandle];
                    cardToHandle->runAction(EaseBackOut::create(MoveTo::create(0.5f, originalPos)));
                }
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

// ��ȡ���Ʒ���
int BoardScene::getCardCost(std::shared_ptr<CardBase> card) {
    return card->cost;
}

int BoardScene::getCardCost(int dbfId) {
    JSONManager manager("cards/json/cards.json");

    return manager.getCardCost(dbfId); 
}

void BoardScene::initPlayers()
{
    localPlayerNumber = photonLib->getLocalPlayerNumber();
    // localPlayerNumber = 1;
    currentPlayerNumber = 1;
    player1 = GameData::getInstance().getPlayer1();
    player2 = GameData::getInstance().getPlayer2();

    // ���� currentPlayerNumber �� localPlayerNumber ���� isLocalPlayerTurn
    isLocalPlayerTurn = (currentPlayerNumber == localPlayerNumber);
}

// ���������ϢUI
void BoardScene::createPlayerUI() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ���屾����ҺͶ��ֵ�ָ��
    players::Player* localPlayer = (localPlayerNumber == 1) ? player1 : player2;
    players::Player* opponentPlayer = (localPlayerNumber == 1) ? player2 : player1;

    // �������������Ϣ��ʾ
    localPlayerHealth = Label::createWithTTF("HP:" + std::to_string(localPlayer->getHealth()), "fonts/arial.ttf", 24);
    localPlayerHealth->setPosition(Vec2(800, 220));
    this->addChild(localPlayerHealth);
    // ������ߣ�ͬʱ������ɫ�ʹ�ϸ��
    localPlayerHealth->enableOutline(Color4B::BLACK, 2);         // ��ɫ��ߣ���ϸΪ2

    localPlayerMana = Label::createWithTTF("Mana:" + std::to_string(localPlayer->getMoney()) + "/" + std::to_string(localPlayer->getMaxMoney()), "fonts/arial.ttf", 24);
    localPlayerMana->setPosition(Vec2(800, 280));
    this->addChild(localPlayerMana);
    // ������ߣ�ͬʱ������ɫ�ʹ�ϸ��
    localPlayerMana->enableOutline(Color4B::BLACK, 2);         // ��ɫ��ߣ���ϸΪ2

    // �����Է������Ϣ��ʾ
    opponentPlayerHealth = Label::createWithTTF("HP:" + std::to_string(opponentPlayer->getHealth()), "fonts/arial.ttf", 24);
    opponentPlayerHealth->setPosition(Vec2(800, visibleSize.height - 220));
    this->addChild(opponentPlayerHealth);
    opponentPlayerHealth->enableOutline(Color4B::BLACK, 2);         // ��ɫ��ߣ���ϸΪ2

    opponentPlayerMana = Label::createWithTTF("Mana:" + std::to_string(opponentPlayer->getMoney()) + "/" + std::to_string(opponentPlayer->getMaxMoney()), "fonts/arial.ttf", 24);
    opponentPlayerMana->setPosition(Vec2(800, visibleSize.height - 280));
    this->addChild(opponentPlayerMana);
    opponentPlayerMana->enableOutline(Color4B::BLACK, 2);         // ��ɫ��ߣ���ϸΪ2

    // �غ�ָʾ��
    turnIndicator = Label::createWithTTF("Your Turn", "fonts/arial.ttf", 32);
    if (isLocalPlayerTurn) {
        turnIndicator->setString("Your Turn");
    }
    else {
        turnIndicator->setString("Opponent's Turn");
    }
    turnIndicator->setPosition(Vec2(1880, 600)); // ���һ��ʵ��һ��
    this->addChild(turnIndicator);
    // ������ߣ�ͬʱ������ɫ�ʹ�ϸ��
    turnIndicator->enableOutline(Color4B::BLACK, 2);         // ��ɫ��ߣ���ϸΪ2

    // ��ӻغϽ�����ť����������ҿ��Կ�����
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

void BoardScene::updatePlayerUI() {
    // ���屾����ҺͶ��ֵ�ָ��
    players::Player* localPlayer = (localPlayerNumber == 1) ? player1 : player2;
    players::Player* opponentPlayer = (localPlayerNumber == 1) ? player2 : player1;

    // ������־
    CCLOG("Updating UI:");
    CCLOG("Local Player (Number %d) - HP: %d, Mana: %d/%d", localPlayerNumber, localPlayer->getHealth(), localPlayer->getMoney(), localPlayer->getMaxMoney());
    CCLOG("Opponent Player (Number %d) - HP: %d, Mana: %d/%d",
        (localPlayerNumber == 1) ? 2 : 1,
        opponentPlayer->getHealth(),
        opponentPlayer->getMoney(),
        opponentPlayer->getMaxMoney());

    // ���±����������ֵ�ͷ���ֵ
    localPlayerHealth->setString("HP: " + std::to_string(localPlayer->getHealth()));
    localPlayerMana->setString("Mana: " + std::to_string(localPlayer->getMoney()) + "/" + std::to_string(localPlayer->getMaxMoney()));

    // ���¶����������ֵ�ͷ���ֵ
    opponentPlayerHealth->setString("HP: " + std::to_string(opponentPlayer->getHealth()));
    opponentPlayerMana->setString("Mana: " + std::to_string(opponentPlayer->getMoney()) + "/" + std::to_string(opponentPlayer->getMaxMoney()));

    // ���»غ�ָʾ
    if (isLocalPlayerTurn) {
        turnIndicator->setString("Your Turn");
    }
    else {
        turnIndicator->setString("Opponent's Turn");
    }

    // �Ӿ��Ϸ�ӳ�غ�״̬
    for (auto& card : localPlayerCards) {
        if (isLocalPlayerTurn) {
            // �ɲ���ʱ���ָ��������
            card->setOpacity(255);
            card->setColor(Color3B::WHITE);
        }
        else {
            // ���ɲ���ʱ������Ϊ��͸�����ɫ
            card->setOpacity(128); // ��͸��
            card->setColor(Color3B(100, 100, 100)); // ��ɫ
        }
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

    photonLib->raiseCustomEvent(eventContent, TURN_START, ExitGames::Lite::ReceiverGroup::ALL);
    CCLOG("Sent TURN_START event for playerNumber: %d", currentPlayerNum);
    cocosUIListener->writeString(EG::JString(L"Sent TURN_START event."));
}

// �ӿ����Ƴ�����ӵ�ս��
void BoardScene::removeCard(cardSprite* sprite) {
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

        // ���ٽ�������ӵ� playedCards�������� handlePlayCard ͳһ����
        sprite->removeFromParent();
        // ������Ч
        audioPlayer("Music/putcard.mp3", false);

        // ����ʣ�࿨�Ƶ�λ��
        Size visibleSize = Director::getInstance()->getVisibleSize();
        for (size_t i = removedIndex; i < localPlayerCards.size(); i++) {
            cardSprite* card = localPlayerCards[i];
            // ������λ��
            Vec2 newPos;
            if (localPlayerNumber == 1) {
                newPos = Vec2(CARD_REGION_X + i * (card->getContentSize().width + 30), CARD_REGION_Y);
            }
            else {
                newPos = Vec2(visibleSize.width - CARD_REGION_X - card->getContentSize().width - i * (card->getContentSize().width + 30), CARD_REGION_Y);
            }

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

// ���³����Ѵ�����Ƶ���ʾ
void BoardScene::updatePlayedCardsPosition() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    float cardWidth = 100;  // ���迨�ƿ��Ϊ100
    float spacing = 120;     // ���Ƽ��

    // ��������Ѵ���Ŀ���
    float startX_local = PLAYED_AREA_PLAYER1_X - (localplayedCards.size() * (cardWidth + spacing) - spacing) / 2;
    for (size_t i = 0; i < localplayedCards.size(); i++) {
        Sprite* card = localplayedCards[i];
        Vec2 targetPos = Vec2(startX_local + i * (cardWidth + spacing), PLAYED_AREA_Y + 100);

        // ʹ�ö����ƶ���Ŀ��λ��
        card->runAction(EaseBackOut::create(MoveTo::create(0.3f, targetPos)));
    }

    // ��������Ѵ���Ŀ���
    float startX_opponent = PLAYED_AREA_PLAYER2_X - (oppentplayedCards.size() * (cardWidth + spacing) - spacing) / 2;
    for (size_t i = 0; i < oppentplayedCards.size(); i++) {
        Sprite* card = oppentplayedCards[i];
        Vec2 targetPos = Vec2(startX_opponent + i * (cardWidth + spacing), PLAYED_AREA_Y - 100);

        // ʹ�ö����ƶ���Ŀ��λ��
        card->runAction(EaseBackOut::create(MoveTo::create(0.3f, targetPos)));
    }
}


// ���ʹ����¼�
///////////////// to be editted
void BoardScene::sendPlayCardEvent(PlayerNumber playerNumber, CardNumber cardNumber) {
    EG::Hashtable eventContent;
    // ʹ�ò�ͬ�� key ������ playerNumber �� cardNumber
    eventContent.put(static_cast<unsigned char>(0), EG::Helpers::ValueToObject<EG::Object>::get(playerNumber));
    eventContent.put(static_cast<unsigned char>(1), EG::Helpers::ValueToObject<EG::Object>::get(cardNumber));

    photonLib->raiseCustomEvent(eventContent, PLAY_CARD, ExitGames::Lite::ReceiverGroup::ALL);
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

// ��������¼�
void BoardScene::handlePlayCard(const EG::Hashtable& parameters) {
    int playerNumber = 0;
    int cardNumber = 0;

    // ��ȡ playerNumber
    const EG::Object* objPlayerNumber = parameters.getValue(static_cast<unsigned char>(0));
    if (objPlayerNumber) {
        const EG::ValueObject<int>* voPlayerNumber = static_cast<const EG::ValueObject<int>*>(objPlayerNumber);
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
        const EG::ValueObject<int>* voCardNumber = static_cast<const EG::ValueObject<int>*>(objCardNumber);
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
    // �۳�Ŀ����ҵķ���ֵ
    int cardCost = getCardCost(cardNumber);
    if (targetPlayer->getMoney() < cardCost) {
        CCLOG("Player %d does not have enough mana to play card %d.", playerNumber, cardNumber);
        cocosUIListener->writeString(EG::JString(L"Player does not have enough mana to play the card."));
        return;
    }
    targetPlayer->setMoney(targetPlayer->getMoney() - cardCost);
    CCLOG("Player %d mana reduced to %d", playerNumber, targetPlayer->getMoney());

    // ����UI
    updatePlayerUI();

    // ��ʾ����Ŀ�����ս����
    addCardToBattlefield(playerNumber, cardNumber);

    // ����Ǳ�����Ҵ���Ŀ��ƣ�����������Ƴ�
    if (playerNumber == localPlayerNumber) {
        cardSprite* card = findCardByID(cardNumber);
        if (card) {
            removeCard(card);
        }
    }
}

// ��ӿ��Ƶ�ս��
void BoardScene::addCardToBattlefield(int playerNumber, int cardNumber) {
    JSONManager manager("cards/json/cards.json");

    // �������ƾ��飬ʹ��������� createWithCard ����
    auto cardData = manager.find_by_dbfId(cardNumber);
    if (!cardData) {
        CCLOG("Card data not found for cardNumber: %d", cardNumber);
        cocosUIListener->writeString(EG::JString(L"Card data not found for cardNumber: ") +
            EG::JString(std::to_wstring(cardNumber).c_str()));
        return;
    }
    Size desiredSize(100, 150); // ������Ҫ����
    // �������ƾ���
    auto battlefieldCard = cardSprite::createWithCard(cardData, desiredSize); // ʹ��������ķ���
    if (battlefieldCard) {
        battlefieldCard->setTag(cardNumber); // ʹ�� cardNumber ��Ϊ tag

        // ����ս���Ͽ��Ƶ�λ��
        Vec2 targetPos;

        if (playerNumber == localPlayerNumber) {
            // ��������Ѵ���Ŀ���
            targetPos = Vec2(PLAYED_AREA_PLAYER1_X + localplayedCards.size() * CARD_SPACING, PLAYED_AREA_Y);
            localplayedCards.push_back(battlefieldCard);
        }
        else {
            // ��������Ѵ���Ŀ���
            targetPos = Vec2(PLAYED_AREA_PLAYER2_X + oppentplayedCards.size() * CARD_SPACING, PLAYED_AREA_Y + 200);
            oppentplayedCards.push_back(battlefieldCard);
        }

        // ��ӵ�ս������¼λ��
        this->addChild(battlefieldCard, 1);

        // ���ó�ʼλ����Ŀ��λ���Ϸ�������
        if (playerNumber == localPlayerNumber) {
            battlefieldCard->setPosition(Vec2(targetPos.x, targetPos.y + 100));
        }
        else {
            battlefieldCard->setPosition(Vec2(targetPos.x, targetPos.y - 100));
        }
        battlefieldCard->setOpacity(0);

        // ������ʾ����
        battlefieldCard->runAction(Sequence::create(
            FadeIn::create(0.5f),
            MoveTo::create(0.5f, targetPos),
            nullptr
        ));
    }
    else {
        CCLOG("Failed to create battlefield card sprite for cardNumber: %d", cardNumber);
        cocosUIListener->writeString(EG::JString(L"Failed to create battlefield card sprite for cardNumber: ") +
            EG::JString(std::to_wstring(cardNumber).c_str()));
    }
}


// ����غϿ�ʼ�¼�
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

    // ��֤ receivedPlayerNumber �Ƿ���Ч
    if (receivedPlayerNumber != 1 && receivedPlayerNumber != 2) {
        CCLOG("Invalid receivedPlayerNumber: %d", receivedPlayerNumber);
        cocosUIListener->writeString(EG::JString(L"Invalid receivedPlayerNumber in TURN_START event."));
        return;
    }

    // ���µ�ǰ��ұ�źͱ��ػغ�״̬
    currentPlayerNumber = receivedPlayerNumber;
    isLocalPlayerTurn = (currentPlayerNumber == localPlayerNumber);

    CCLOG("currentPlayerNumber set to: %d, isLocalPlayerTurn: %s", currentPlayerNumber, isLocalPlayerTurn ? "true" : "false");

    // ȷ����ǰ�غϵ����
    players::Player* currentPlayer = (currentPlayerNumber == 1) ? player1 : player2;
    currentPlayer->increaseMana();
    CCLOG("Player %d mana increased to %d", currentPlayerNumber, currentPlayer->getMoney());

    // ����UI
    updatePlayerUI();

    // ����Ǳ�����ҵĻغϣ���һ�ſ���
    if (isLocalPlayerTurn) {
        if (currentPlayer->hasCards()) {
            std::shared_ptr<CardBase> card = currentPlayer->drawCard(); // ��ȡһ�ſ���
            if (card) {
                addCardToLocalPlayer(card);
                CCLOG("Player %d drew cardNumber: %d", currentPlayerNumber, card->dbfId);
                cocosUIListener->writeString(EG::JString(L"Player drew a card."));
            }
        }
        else {
            CCLOG("Player %d has no cards to draw. Ending game.", currentPlayerNumber);
            cocosUIListener->writeString(EG::JString(L"Player has no cards to draw. Game Over."));
            endGame((currentPlayerNumber == 1) ? player2 : player1);
        }
    }

    cocosUIListener->writeString(EG::JString(L"Handled TURN_START event. Switched turn."));
}

// �ַ���ʼ����
void BoardScene::distributeInitialHands()
{
    // Ϊ������ҳ�ȡ��ʼ����
    for (int i = 0; i < 3; ++i) {
        players::Player* localPlayer = (localPlayerNumber == 1) ? player1 : player2;
        std::shared_ptr<CardBase> card = localPlayer->drawCard();
        if (card) {
            CCLOG("Distributing initial cardNumber: %d to playerNumber: %d", card->dbfId, localPlayerNumber);
            addCardToLocalPlayer(card);
            cocosUIListener->writeString(EG::JString(L"Player drew initial card."));
        }
        else {
            CCLOG("Player %d failed to draw initial card.", localPlayer->getPlayerNumber());
            cocosUIListener->writeString(EG::JString(L"Failed to draw initial card."));
        }
    }
}

// ��ӿ��Ƶ�������ң���������ʾ��
void BoardScene::addCardToLocalPlayer(std::shared_ptr<CardBase> card) {
    players::Player* localPlayer = (localPlayerNumber == 1) ? player1 : player2;
    // localPlayer->addCardToHand(card);

    Size desiredSize(100, 150);

    cardSprite* newCard = cardSprite::createWithCard(card, desiredSize);

    if (newCard) {
        CCLOG("Successfully created cardSprite for cardNumber: %d", card->dbfId);
        newCard->setTag(card->dbfId); // ʹ�� cardNumber ��Ϊ tag

        //float xPos = CARD_REGION_X + localPlayerCards.size() * (newCard->getContentSize().width + 30);
        Vec2 originalPos(CARD_REGION_X + localPlayerCards.size() * (newCard->getContentSize().width + 30),
            CARD_REGION_Y);
        //Vec2 originalPos(xPos, CARD_REGION_Y);
        CCLOG("Setting card position to: (%f, %f)", originalPos.x, originalPos.y);

        // ��ӵ����������򲢼�¼λ��
        this->addChild(newCard);
        cardOriginalPositions[newCard] = originalPos;
        localPlayerCards.push_back(newCard);

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

        // ����UI
        updatePlayerUI();

        // ��־
        cocosUIListener->writeString(EG::JString(L"Added initial card locally."));
    }
    else {
        CCLOG("Failed to create card sprite for cardNumber: %d", card->dbfId);
        cocosUIListener->writeString(EG::JString(L"Failed to create card sprite for cardNumber: ") +
            EG::JString(std::to_wstring(card->dbfId).c_str()));
    }
}


// �������������ݿ���ID���Ҿ���
cardSprite* BoardScene::findCardByID(int cardNumber) {
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
    // ���� PhotonLib ���Զ����¼��ص�
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (photonLib) {
        photonLib->setCustomEventCallback(nullptr);
    }
    // �����ƾ���
    for (auto& card : localPlayerCards) {
        if (card) {
            card->removeFromParent();
        }
    }
    localPlayerCards.clear();

    for (auto& card : localplayedCards) {
        if (card) {
            card->removeFromParent();
        }
    }
    localplayedCards.clear();

    for (auto& card : oppentplayedCards) {
        if (card) {
            card->removeFromParent();
        }
    }
    oppentplayedCards.clear();

    // ������λ�ü�¼
    cardOriginalPositions.clear();

    player1 = nullptr;
    player2 = nullptr;
    CCLOG("BoardScene destroyed and all card references cleared.");
}


///Ŀǰ���� UI�����䣬ԭ���Ŀ������Ȼ���ڣ�ͬʱû����ȷ��ʵ�ֶ��ֿ��Ƶ���ʾ