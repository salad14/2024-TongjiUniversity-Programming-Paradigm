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
    localMinionCard.clear();
    oppentMinionCard.clear();

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
        cancel->setPosition(Vec2(1940, 80)); // ���һ��ʵ��һ��
    }
    auto menu = Menu::create(cancel, nullptr);

    // ���Ӳ˵�
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

// �����з���ӣ��������ԣ���ʼ����4��������ڳ��ϣ�
//void BoardScene::initEnemyCards() {
//    // ��ʼ��4�ŵз���ӿ���
//    for (int i = 0; i < 4; i++) {
//        auto cardData = std::make_shared<MinionCard>();
//        cardData->dbfId = 67; // ʾ������ID��ȷ�� 67.png ������ Resources/cards/
//        // ����̶���С��������Ҫ������
//        Size desiredSize(120, 180);
//
//        // ʹ��������� createWithCard �����������ƾ��飬������������С
//        auto card = cardSprite::createWithCard(cardData, desiredSize);
//        if (card) {
//            // ���ÿ��Ƶ����ţ������Ҫ��һ��������
//            card->setScale(0.8f); // ������� createWithCard �е�������С����ʡ��
//
//            // �������Ա�ǩ��Ѫ��5��������1������1��
//            addCardStats(card, 5, 1, 1);
//
//            // ���ӵ��з��ѳ��ƿ����б�
//            oppentMinionCard.push_back(card);
//
//            // ���ӵ������У����� z-order Ϊ 50��������Ҫ������
//            this->addChild(card, 50);
//        }
//        else {
//            CCLOG("Failed to create enemy card sprite for cardNumber: %d", cardData->dbfId);
//        }
//        updateEnemyCardsPosition();
//    }
//
//}

// ���ӿ��Ƶ����Ա�ǩ
void BoardScene::addCardStats(cardSprite* newcard) {
    if (!newcard) return;
    // auto minionCard = std::dynamic_pointer_cast<MinionCard>(newcard->card);
    // if (!minionCard) throw std::runtime_error("Attacker is not a minion card");

    // �������������Ա�ǩ
    auto healthLabel = Label::createWithTTF(std::to_string(newcard->currentHealth), "fonts/arial.ttf", 24);
    healthLabel->setPosition(Vec2(newcard->getContentSize().width - 20, 20));
    healthLabel->setName("healthLabel");
    healthLabel->enableOutline(Color4B::BLACK, 2);
    newcard->addChild(healthLabel);

    auto attackLabel = Label::createWithTTF(std::to_string(newcard->currentAttack), "fonts/arial.ttf", 24);
    attackLabel->setPosition(Vec2(20, 20));
    attackLabel->setName("attackLabel");
    attackLabel->enableOutline(Color4B::BLACK, 2);
    newcard->addChild(attackLabel);

    //auto costLabel = Label::createWithTTF(std::to_string(minionCard->cost), "fonts/arial.ttf", 24);
    //costLabel->setPosition(Vec2(20, newcard->getContentSize().height - 20));
    //costLabel->setName("costLabel");
    //costLabel->enableOutline(Color4B::BLACK, 2);
    //newcard->addChild(costLabel);
}

// ���¿����ϵ����Ա�ǩ
void BoardScene::updateCardStats(cardSprite* card)
{
    if (!card) return;
    // ��ȡ��ǩ
    auto healthLabel = dynamic_cast<Label*>(card->getChildByName("healthLabel"));
    auto attackLabel = dynamic_cast<Label*>(card->getChildByName("attackLabel"));
    //auto costLabel = dynamic_cast<Label*>(newcard->getChildByName("costLabel"));
    // ���±�ǩ
    if (healthLabel && card->currentHealth > 0) {
        healthLabel->setString(std::to_string(card->currentHealth));
    }
    if (attackLabel) {
        attackLabel->setString(std::to_string(card->currentAttack));
    }
    //if (costLabel) {
    //    costLabel->setString(std::to_string(minionCard->cost));
    //}
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
}

// ��׼ ѡ�񹥻�����
void BoardScene::createAttackIndicator(const Vec2& startPos) {
    if (attackIndicator) {
        attackIndicator->removeFromParent();
    }

    attackIndicator = DrawNode::create();
    this->addChild(attackIndicator, 100);

    // ���ƺ�ɫ��׼Ȧ
    attackIndicator->drawCircle(startPos, 30, 0, 360, false, Color4F(1, 0, 0, 0.8f));
}

// ����������ͨ�������ң������ҹ��ã� // Ĭ��attacker�Ǽ���
void BoardScene::attackmove(PlayerNumber player, int attackerIndex, int defenderIndex) {
    if (attackerIndex < 0 || defenderIndex < 0) { return; }

    cardSprite* attacker = nullptr;
    cardSprite* defender = nullptr;
    if (player == localPlayerNumber) { // �����ǹ�����
        if (attackerIndex >= localMinionCard.size() || defenderIndex >= oppentMinionCard.size())
            throw std::runtime_error("out of index");
        attacker = localMinionCard[attackerIndex];
        defender = oppentMinionCard[defenderIndex];
    } else { //�Է��ǹ�����
        if (defenderIndex >= localMinionCard.size() || attackerIndex >= oppentMinionCard.size())
            throw std::runtime_error("out of index");
        attacker = oppentMinionCard[defenderIndex];
        defender = localMinionCard[attackerIndex];
    }

    // ���湥���ߵ�ԭʼλ��
    Vec2 originalPos = attacker->getPosition();
    // ��ȡĿ��λ��
    Vec2 targetPos = defender->getPosition();
    // ����������������
    attacker->runAction(Sequence::create(
        // �����ƶ���Ŀ��λ��
        EaseIn::create(MoveTo::create(0.2f, targetPos), 2.0f),
        // ����һ���̵ܶ�ͣ��
        DelayTime::create(0.1f),
        // ����ԭʼλ��
        EaseOut::create(MoveTo::create(0.2f, originalPos), 2.0f),
        nullptr
    ));
    // ��������һ���򵥵��ܻ�Ч��
    defender->runAction(Sequence::create(
        // �ζ�Ч��
        RotateBy::create(0.1f, 10),
        RotateBy::create(0.1f, -20),
        RotateBy::create(0.1f, 10),
        nullptr
    ));
}

// ������Ӷ���ӵĹ���
//////////////// û�з��͹����ź�
//void BoardScene::handleMinionAttackMinion(int attackerIndex, int defenderIndex) {
//    if (attackerIndex == -1 || defenderIndex == -1) return;
//    
//    // ת��Ϊ����ָ�봦��
//    auto attacker = localMinionCard[attackerIndex];
//    auto defender = oppentMinionCard[defenderIndex];
//
//    // �����˺�
//    attacker->currentHealth -= defender->currentAttack;
//    defender->currentHealth -= attacker->currentAttack;
//
//    // UI���� ���Ź������� ���¿���UI
//    attackmove(attackerIndex, defenderIndex);
//    updateCardStats(defender);
//    updateCardStats(attacker);
//    //////// �����ܷ��һ��ʱ���ӳٹ��ܣ�  չʾ�������
//
//    // ����������
//    checkMinionDie(attacker);
//    checkMinionDie(defender);
//
//    // ����UI
//    updatePlayerUI();
//
//    // ���Ź�����Ч
//    audioPlayer("Music/attack.mp3", false);
//}

// ������Ӣ�۵Ĺ���
//////////////////////// ��Ҫ�ȶ��幥���ź�
void BoardScene::handleMinionAttackHero() {
    // ���Ź�����Ч
    audioPlayer("Music/attack.mp3", false);

    //����ʵ��һ��������Ч��
    // ��ȡ�������飨ͨ��tag�����֣�
    auto background = this->getChildByTag(0);  // ֮ǰ���ӱ���ʱ���õ�tagΪ0
    // ����ͨ�����ֻ�ȡ
    // auto background = this->getChildByName("background");

    if (background) {
        // ����ԭʼλ��
        Vec2 originalPos = background->getPosition();

        // ����������
        auto shake = Sequence::create(
            MoveBy::create(0.02f, Vec2(-10, -5)),
            MoveBy::create(0.02f, Vec2(20, 10)),
            MoveBy::create(0.02f, Vec2(-20, -10)),
            MoveBy::create(0.02f, Vec2(20, 10)),
            MoveBy::create(0.02f, Vec2(-10, -5)),
            // ȷ���ص�ԭʼλ��
            MoveTo::create(0, originalPos),
            nullptr
        );

        background->runAction(shake);
    }
}

// �Ƴ����Ʋ����Ŷ���
void BoardScene::removeCardWithAnimation(cardSprite* card) {
    if (!card) return;
    // �ȴ� cardStats ���Ƴ�
    // cardStats.erase(card);

    // ������ʧ����
    card->runAction(Sequence::create(
        Spawn::create(
            FadeOut::create(0.5f),
            ScaleTo::create(0.5f, 0.1f),
            nullptr
        ),
        CallFunc::create([this, card]() {
            // ���������Ƴ�
            auto it = std::find(oppentMinionCard.begin(), oppentMinionCard.end(), card);
            if (it != oppentMinionCard.end()) {
                oppentMinionCard.erase(it);
            }
            it = std::find(localMinionCard.begin(), localMinionCard.end(), card);
            if (it != localMinionCard.end()) {
                localMinionCard.erase(it);
            }
            card->removeFromParent();
            updatePlayedCardsPosition();
            updateEnemyCardsPosition();
            }),
        nullptr
    ));

    // �����Ƴ���Ч
    audioPlayer("Music/broken.mp3", false);
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

    // ����Ѿ�ѡ���˹�����ӣ�����Ƿ�������ЧĿ��
    if (attackingCard) {
        // ����Ƿ����˵з���ӻ�з�Ӣ������
        bool targetFound = false;

        // ���з����
        for (int i = 0; i < oppentMinionCard.size(); ++i) {
            cardSprite* enemyCard = oppentMinionCard[i];
            if (enemyCard->getBoundingBox().containsPoint(touchLocation)) {
                sendMinionAttackEvent(localPlayerNumber, get_localMinionIndex(attackingCard), i);
                targetFound = true;
                break;
            }
        }

        // ���з�Ӣ������
        if (!targetFound && touchLocation.y > Director::getInstance()->getVisibleSize().height * 0.7f) {
            sendMinionAttackEvent(localPlayerNumber, get_localMinionIndex(attackingCard), -1);
        }

        // �������״̬
        attackingCard = nullptr;
        if (attackIndicator) {
            attackIndicator->removeFromParent();
            attackIndicator = nullptr;
        }
        return true;
    }

    // ����Ƿ����˼������
    for (auto card : localMinionCard) {
        if (card->getBoundingBox().containsPoint(touchLocation)) {
            attackingCard = card;
            createAttackIndicator(card->getPosition());
            return true;
        }
    }

    // �������
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

void BoardScene::onTouchMoved(Touch* touch, Event* event) {
    if (selectedCard) {
        selectedCard->setPosition(selectedCard->getPosition() + touch->getDelta());
        checkDropArea();
    }
    else if (attackingCard && attackIndicator) {
        // ���¹���ָʾ��λ��
        attackIndicator->clear();
        Vec2 startPos = attackingCard->getPosition();
        Vec2 currentPos = touch->getLocation();

        // ���ƹ�����
        attackIndicator->drawLine(startPos, currentPos, Color4F(1, 0, 0, 0.8f));
        // ������׼Ȧ
        attackIndicator->drawCircle(currentPos, 30, 0, 360, false, Color4F(1, 0, 0, 0.8f));
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
                int cardCost = jsonmanager.getCardCost(cardToHandle->card->cost);
                if (currentPlayer->getMoney() >= cardCost) { // ʹ�� getter ����
                    // �۳�����ֵ
                    currentPlayer->setMoney(currentPlayer->getMoney() - cardCost);
                    CCLOG("Player %d mana reduced to %d", localPlayerNumber, currentPlayer->getMoney());

                    // ����UI
                    updatePlayerUI();

                    if (cardToHandle->card->type == cardType::MINION) {
                        // ���� PLAY_MINION_CARD �¼�
                        sendPlay_MinionCardEvent(localPlayerNumber, cardToHandle->card->dbfId);
                        // ��ʾ������ս����
                        add_HandCardToBattlefield(cardToHandle);
                        // ���������Ƴ�����
                        removeCard(cardToHandle);
                    }
                    else if (cardToHandle->card->type == cardType::SPELL) {
                        // ���� PLAY_SPELL_CARD �¼�
                        sendPlay_SpellCardEvent(localPlayerNumber, cardToHandle->card->dbfId);
                        
                        
                        //1. ����Ч���ź�
                        //2./////////// ������Ҫ���������������߼� \\\\\\\\\\\\\\\\
                        

                    }
                }
                else
                {
                    returnCardToHand(cardToHandle);
                }
            }
        }
        else
        {
            returnCardToHand(cardToHandle);
        }
    }
}

// ȡ��ʹ���ƣ� �����ƷŻ�����
void BoardScene::returnCardToHand(cardSprite* card) {
    Vec2 originalPos = cardOriginalPositions[card];
    card->runAction(Sequence::create(
        EaseBackOut::create(MoveTo::create(0.5f, originalPos)),
        CallFunc::create([card]() {
            card->setColor(Color3B::WHITE);
            card->setOpacity(255);
            }),
        nullptr
    ));
    if (hoveredCard == card) {
        scaleSprite(card, 1.5f);
    }
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

// �ӿ����Ƴ������ӵ�ս��
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

        // ���ٽ��������ӵ� playedCards�������� handle_PlayMinionCard ͳһ����
        sprite->removeFromParent();
        // ������Ч
        audioPlayer("Music/putcard.mp3", false);

        // ����ʣ�࿨�Ƶ�λ��
        Size visibleSize = Director::getInstance()->getVisibleSize();
        for (size_t i = removedIndex; i < localPlayerCards.size(); i++) {
            cardSprite* card = localPlayerCards[i];
            Vec2 newPos(CARD_REGION_X + i * (card->getContentSize().width + 30), CARD_REGION_Y);
            cardOriginalPositions[card] = newPos;

            card->runAction(Sequence::create(
                EaseInOut::create(MoveTo::create(0.3f, newPos), 2.0f),
                EaseElasticOut::create(ScaleTo::create(0.2f, 1.0f)),
                nullptr
            ));
        }
    }
}

// ���¼�����ӵ�λ��
void BoardScene::updatePlayedCardsPosition() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height * 0.45f);
    float cardWidth = 100;
    float spacing = 20;

    float startX = center.x - (localMinionCard.size() * (cardWidth + spacing) - spacing) / 2;

    for (size_t i = 0; i < localMinionCard.size(); i++) {
        Sprite* card = localMinionCard[i];
        Vec2 targetPos = Vec2(startX + i * (cardWidth + spacing), center.y);
        card->runAction(EaseBackOut::create(MoveTo::create(0.3f, targetPos)));
    }
}

// ���µ��˵����λ��
void BoardScene::updateEnemyCardsPosition() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height * 0.6f);
    float cardWidth = 100;
    float spacing = 20;

    float startX = center.x - (oppentMinionCard.size() * (cardWidth + spacing) - spacing) / 2;

    for (size_t i = 0; i < oppentMinionCard.size(); i++) {
        Sprite* card = oppentMinionCard[i];
        Vec2 targetPos = Vec2(startX + i * (cardWidth + spacing), center.y);
        card->runAction(EaseBackOut::create(MoveTo::create(0.3f, targetPos)));
    }
}

// ���ʹ�������¼� been editted
void BoardScene::sendPlay_MinionCardEvent(PlayerNumber playerNumber, CardNumber dbfID) {
    EG::Hashtable eventContent;
    // ʹ�ò�ͬ�� key ������ playerNumber �� cardID
    eventContent.put(static_cast<unsigned char>(0), EG::Helpers::ValueToObject<EG::Object>::get(playerNumber));
    eventContent.put(static_cast<unsigned char>(1), EG::Helpers::ValueToObject<EG::Object>::get(dbfID));

    photonLib->raiseCustomEvent(eventContent, PLAY_MINION_CARD, ExitGames::Lite::ReceiverGroup::OTHERS); // �޸�Ϊֻ�жԷ������ź�
    CCLOG("Sent PLAY_MINION_CARD event with playerNumber: %d, cardID: %d", playerNumber, dbfID);
    cocosUIListener->writeString(EG::JString(L"Sent PLAY_MinionCard event with playerNumber: ") +
        EG::JString(std::to_wstring(playerNumber).c_str()) +
        EG::JString(L", cardID: ") +
        EG::JString(std::to_wstring(dbfID).c_str()));
}

// ���ʹ���������ź�
void BoardScene::sendPlay_SpellCardEvent(PlayerNumber playerNumber, CardNumber dbfID) {
    EG::Hashtable eventContent;
    // ʹ�ò�ͬ�� key ������ playerNumber �� cardID
    eventContent.put(static_cast<unsigned char>(0), EG::Helpers::ValueToObject<EG::Object>::get(playerNumber));
    eventContent.put(static_cast<unsigned char>(1), EG::Helpers::ValueToObject<EG::Object>::get(dbfID));

    // ʹ�� ReceiverGroup::OTHERS ֻ���͸��������
    photonLib->raiseCustomEvent(eventContent, PLAY_SPELL_CARD, ExitGames::Lite::ReceiverGroup::OTHERS);
    CCLOG("Sent PLAY_SPELL_CARD event with playerNumber: %d, cardID: %d", playerNumber, dbfID);
    cocosUIListener->writeString(EG::JString(L"Sent PLAY_SpellCard event with playerNumber: ") +
        EG::JString(std::to_wstring(playerNumber).c_str()) +
        EG::JString(L", cardID: ") +
        EG::JString(std::to_wstring(dbfID).c_str()));
}

// ��������ƹ������ź�
void BoardScene::sendMinionAttackEvent(PlayerNumber playerNumber, int attackerIndex, int defenderIndex) {
    EG::Hashtable eventContent;
    // ʹ�ò�ͬ�� key �����ֹ�����ҡ������������ͷ���������
    eventContent.put(static_cast<unsigned char>(0), EG::Helpers::ValueToObject<EG::Object>::get(playerNumber));
    eventContent.put(static_cast<unsigned char>(1), EG::Helpers::ValueToObject<EG::Object>::get(attackerIndex));
    eventContent.put(static_cast<unsigned char>(2), EG::Helpers::ValueToObject<EG::Object>::get(defenderIndex));

    // ʹ�� ReceiverGroup::OTHERS ֻ���͸��������
    photonLib->raiseCustomEvent(eventContent, MINION_ATTACK, ExitGames::Lite::ReceiverGroup::OTHERS);

    CCLOG("Sent MINION_ATTACK_EVENT with playerNumber: %d, attackerIndex: %d, defenderIndex: %d", playerNumber, attackerIndex, defenderIndex);
    cocosUIListener->writeString(EG::JString(L"Sent MINION_ATTACK_EVENT with playerNumber: ") +
        EG::JString(std::to_wstring(playerNumber).c_str()) +
        EG::JString(L", attackerIndex: ") +
        EG::JString(std::to_wstring(attackerIndex).c_str()) +
        EG::JString(L", defenderIndex: ") +
        EG::JString(std::to_wstring(defenderIndex).c_str()));
}

// ���ͷ��������¼�
void BoardScene::sendSpellAttackEvent(PlayerNumber attackPlayer, int defenderIndex, int dbfId) {
    EG::Hashtable eventContent;
    // ʹ�ò�ͬ�� key �����ֹ�����ҡ��ܹ�����Һͷ������Ʊ��
    eventContent.put(static_cast<unsigned char>(0), EG::Helpers::ValueToObject<EG::Object>::get(attackPlayer));
    eventContent.put(static_cast<unsigned char>(1), EG::Helpers::ValueToObject<EG::Object>::get(defenderIndex));
    eventContent.put(static_cast<unsigned char>(2), EG::Helpers::ValueToObject<EG::Object>::get(dbfId));

    // ʹ�� ReceiverGroup::OTHERS ֻ���͸��������
    photonLib->raiseCustomEvent(eventContent, SPELL_ATTACK, ExitGames::Lite::ReceiverGroup::ALL);

    CCLOG("Sent SPELL_ATTACK_EVENT with attackPlayer: %d, defenderIndex: %d, cardID: %d", attackPlayer, defenderIndex, dbfId);
    cocosUIListener->writeString(EG::JString(L"Sent SPELL_ATTACK_EVENT with attackPlayer: ") +
        EG::JString(std::to_wstring(attackPlayer).c_str()) +
        EG::JString(L", defenderIndex: ") +
        EG::JString(std::to_wstring(defenderIndex).c_str()) +
        EG::JString(L", cardID: ") +
        EG::JString(std::to_wstring(dbfId).c_str()));
}

// ���� Photon �Զ����¼�
void BoardScene::onPhotonEvent(int eventCode, const EG::Hashtable& parameters) {
    switch (eventCode) {
        case PLAY_MINION_CARD:
            handle_PlayMinionCard(parameters);
            break;
        case TURN_START:
            handle_TurnStart(parameters);
            break;
        case PLAY_SPELL_CARD:
            handle_PlaySpellCard(parameters);
            break;
        case SPELL_ATTACK:
        case DRAW_CARD:
        case MINION_ATTACK:
        default:
            CCLOG("Received unknown eventCode: %d", eventCode);
            cocosUIListener->writeString(EG::JString(L"Received unknown eventCode: ") +
                EG::JString(std::to_wstring(eventCode).c_str()));
            break;
    }
}

// ������������¼�
void BoardScene::handle_PlayMinionCard(const EG::Hashtable& parameters) {
    int playerNumber = 0;
    int dbfId = 0;

    // ��ȡ playerNumber
    const EG::Object* objPlayerNumber = parameters.getValue(static_cast<unsigned char>(0));
    if (objPlayerNumber) {
        const EG::ValueObject<int>* voPlayerNumber = static_cast<const EG::ValueObject<int>*>(objPlayerNumber);
        if (voPlayerNumber) {
            playerNumber = voPlayerNumber->getDataCopy();
            CCLOG("Received PLAY_MINION_CARD for playerNumber: %d", playerNumber);
        }
        else {
            CCLOG("Failed to cast playerNumber in PLAY_MINION_CARD event.");
            cocosUIListener->writeString(EG::JString(L"Failed to cast playerNumber in PLAY_MINION_CARD event."));
            return;
        }
    }
    else {
        CCLOG("playerNumber not found in PLAY_MINION_CARD event.");
        cocosUIListener->writeString(EG::JString(L"playerNumber not found in PLAY_MINION_CARD event."));
        return;
    }

    // ��ȡ cardNumber
    const EG::Object* objCardNumber = parameters.getValue(static_cast<unsigned char>(1));
    if (objCardNumber) {
        const EG::ValueObject<int>* voCardNumber = static_cast<const EG::ValueObject<int>*>(objCardNumber);
        if (voCardNumber) {
            dbfId = voCardNumber->getDataCopy();
            CCLOG("Received PLAY_MINION_CARD with dbfId: %d for playerNumber: %d", dbfId, playerNumber);
        }
        else {
            CCLOG("Failed to cast dbfId in PLAY_MINION_CARD event.");
            cocosUIListener->writeString(EG::JString(L"Failed to cast dbfId in PLAY_MINION_CARD event."));
            return;
        }
    }
    else {
        CCLOG("dbfId not found in PLAY_MINION_CARD event.");
        cocosUIListener->writeString(EG::JString(L"dbfId not found in PLAY_MINION_CARD event."));
        return;
    }

    // ����Ǽ����ĳ�������źţ����ﲻ������
    if (playerNumber == localPlayerNumber) return;

    // ȷ��Ŀ�����
    players::Player* targetPlayer = nullptr;
    if (playerNumber == 1) {
        targetPlayer = player1;
    }
    else if (playerNumber == 2) {
        targetPlayer = player2;
    }
    // �۳�Ŀ����ҵķ���ֵ
    int cardCost = jsonmanager.getCardCost(dbfId);
    if (targetPlayer->getMoney() < cardCost) {
        CCLOG("Player %d does not have enough mana to play card %d.", playerNumber, dbfId);
        cocosUIListener->writeString(EG::JString(L"Player does not have enough mana to play the card."));
        return;
    }
    targetPlayer->setMoney(targetPlayer->getMoney() - cardCost);
    CCLOG("Player %d mana reduced to %d", playerNumber, targetPlayer->getMoney());

    // ����UI
    updatePlayerUI();

    if (playerNumber != localPlayerNumber) {
        // ��ʾ����Ŀ�����ս����
        add_NewCardToBattlefield(playerNumber, dbfId);
    }

    // ����Ǳ�����Ҵ���Ŀ��ƣ�����������Ƴ� // ���ⲿ�����Ѿ��Ƴ���һ�� ����Ҫ����
    //if (playerNumber == localPlayerNumber) {
    //    cardSprite* card = findCardByID(cardNumber);
    //    if (card) {
    //        removeCard(card);
    //    }
    //}
}

// ��������������¼�
void BoardScene::handle_PlaySpellCard(const EG::Hashtable& parameters) {
    int playerNumber = 0;
    int dbfId = 0;

    // ��ȡ playerNumber
    const EG::Object* objPlayerNumber = parameters.getValue(static_cast<unsigned char>(0));
    if (objPlayerNumber) {
        const EG::ValueObject<int>* voPlayerNumber = static_cast<const EG::ValueObject<int>*>(objPlayerNumber);
        if (voPlayerNumber) {
            playerNumber = voPlayerNumber->getDataCopy();
            CCLOG("Received PLAY_SPELL_CARD for playerNumber: %d", playerNumber);
        }
        else {
            CCLOG("Failed to cast playerNumber in PLAY_SPELL_CARD event.");
            cocosUIListener->writeString(EG::JString(L"Failed to cast playerNumber in PLAY_SPELL_CARD event."));
            return;
        }
    }
    else {
        CCLOG("playerNumber not found in PLAY_SPELL_CARD event.");
        cocosUIListener->writeString(EG::JString(L"playerNumber not found in PLAY_SPELL_CARD event."));
        return;
    }

    // ��ȡ cardNumber
    const EG::Object* objCardNumber = parameters.getValue(static_cast<unsigned char>(1));
    if (objCardNumber) {
        const EG::ValueObject<int>* voCardNumber = static_cast<const EG::ValueObject<int>*>(objCardNumber);
        if (voCardNumber) {
            dbfId = voCardNumber->getDataCopy();
            CCLOG("Received PLAY_SPELL_CARD with dbfId: %d for playerNumber: %d", dbfId, playerNumber);
        }
        else {
            CCLOG("Failed to cast dbfId in PLAY_SPELL_CARD event.");
            cocosUIListener->writeString(EG::JString(L"Failed to cast dbfId in PLAY_SPELL_CARD event."));
            return;
        }
    }
    else {
        CCLOG("dbfId not found in PLAY_SPELL_CARD event.");
        cocosUIListener->writeString(EG::JString(L"dbfId not found in PLAY_SPELL_CARD event."));
        return;
    }

    // ����Ǽ����ĳ��������źţ����ﲻ������
    if (playerNumber == localPlayerNumber) return;

    // ����ǶԷ����������
    // UI����
    // ShowOpponentPlayedCard(dbfId);
}

// �������������¼�
void BoardScene::handle_SpellAttackEvent(const EG::Hashtable& parameters) {
    int playerNumber = 0;
    int defenderIndex = 0;
    int damage = 0;

    // ��ȡ playerNumber
    const EG::Object* objPlayerNumber = parameters.getValue(static_cast<unsigned char>(0));
    if (objPlayerNumber) {
        const EG::ValueObject<int>* voPlayerNumber = static_cast<const EG::ValueObject<int>*>(objPlayerNumber);
        if (voPlayerNumber) {
            playerNumber = voPlayerNumber->getDataCopy();
            CCLOG("Received SPELL_ATTACK_EVENT for playerNumber: %d", playerNumber);
        }
        else {
            CCLOG("Failed to cast playerNumber in SPELL_ATTACK_EVENT.");
            cocosUIListener->writeString(EG::JString(L"Failed to cast playerNumber in SPELL_ATTACK_EVENT."));
            return;
        }
    }
    else {
        CCLOG("playerNumber not found in SPELL_ATTACK_EVENT.");
        cocosUIListener->writeString(EG::JString(L"playerNumber not found in SPELL_ATTACK_EVENT."));
        return;
    }

    // ��ȡ defenderIndex
    const EG::Object* objDefenderIndex = parameters.getValue(static_cast<unsigned char>(1));
    if (objDefenderIndex) {
        const EG::ValueObject<int>* voDefenderIndex = static_cast<const EG::ValueObject<int>*>(objDefenderIndex);
        if (voDefenderIndex) {
            defenderIndex = voDefenderIndex->getDataCopy();
            CCLOG("Received SPELL_ATTACK_EVENT with defenderIndex: %d for playerNumber: %d", defenderIndex, playerNumber);
        }
        else {
            CCLOG("Failed to cast defenderIndex in SPELL_ATTACK_EVENT.");
            cocosUIListener->writeString(EG::JString(L"Failed to cast defenderIndex in SPELL_ATTACK_EVENT."));
            return;
        }
    }
    else {
        CCLOG("defenderIndex not found in SPELL_ATTACK_EVENT.");
        cocosUIListener->writeString(EG::JString(L"defenderIndex not found in SPELL_ATTACK_EVENT."));
        return;
    }

    // ��ȡ damage
    const EG::Object* objDamage = parameters.getValue(static_cast<unsigned char>(2));
    if (objDamage) {
        const EG::ValueObject<int>* voDamage = static_cast<const EG::ValueObject<int>*>(objDamage);
        if (voDamage) {
            damage = voDamage->getDataCopy();
            CCLOG("Received SPELL_ATTACK_EVENT with damage: %d", damage);
        }
        else {
            CCLOG("Failed to cast damage in SPELL_ATTACK_EVENT.");
            cocosUIListener->writeString(EG::JString(L"Failed to cast damage in SPELL_ATTACK_EVENT."));
            return;
        }
    }
    else {
        CCLOG("damage not found in SPELL_ATTACK_EVENT.");
        cocosUIListener->writeString(EG::JString(L"damage not found in SPELL_ATTACK_EVENT."));
        return;
    }

    auto targetplayer = (localPlayerNumber == 1) ? player2 : player1;
    auto localplayer = (localPlayerNumber == 1) ? player1 : player2;
    cardSprite* defender = NULL;
    if (playerNumber == localPlayerNumber) { // �������������
        if (defenderIndex == -1) { // ����Ӣ��
            if (!targetplayer->getDamage(damage)) { // �Ƿ�����
                endGame(localplayer);
                cocosUIListener->writeString(EG::JString(L"Hero is dead."));
            }
            updatePlayerUI();
            return;
        } else {
            // ��ȡ�Է�������λ
            defender = oppentMinionCard[defenderIndex];
            defender->getDamage(damage);
            checkMinionDie(defender);
            updateCardStats(defender);
            // ������������
            // .....
        }
    }
    else { // �Է����������
        defender = localMinionCard[defenderIndex];
        if (defenderIndex == -1) { // ����Ӣ��
            if (!localplayer->getDamage(damage)) { // �Ƿ�����
                endGame(targetplayer);
                cocosUIListener->writeString(EG::JString(L"Hero is dead."));
            }
            updatePlayerUI();
            return;
        }
        else {
            defender = localMinionCard[defenderIndex];
            defender->getDamage(damage);
            checkMinionDie(defender);
            updateCardStats(defender);
            // ������������
            // .....
        }

    }
}

// ������ӹ����¼�
void BoardScene::handle_MinionAttackEvent(const ExitGames::Common::Hashtable& parameters) {
    int playerNumber = 0;
    int attackerIndex = -1;
    int defenderIndex = -1;

    // ��ȡ playerNumber
    const EG::Object* objPlayerNumber = parameters.getValue(static_cast<unsigned char>(0));
    if (objPlayerNumber) {
        const EG::ValueObject<int>* voPlayerNumber = static_cast<const EG::ValueObject<int>*>(objPlayerNumber);
        if (voPlayerNumber) {
            playerNumber = voPlayerNumber->getDataCopy();
            CCLOG("Received MINION_ATTACK_EVENT for playerNumber: %d", playerNumber);
        }
        else {
            CCLOG("Failed to cast playerNumber in MINION_ATTACK_EVENT.");
            cocosUIListener->writeString(EG::JString(L"Failed to cast playerNumber in MINION_ATTACK_EVENT."));
            return;
        }
    }
    else {
        CCLOG("playerNumber not found in MINION_ATTACK_EVENT.");
        cocosUIListener->writeString(EG::JString(L"playerNumber not found in MINION_ATTACK_EVENT."));
        return;
    }

    // ��ȡ attackerIndex��������������
    const EG::Object* objAttackerIndex = parameters.getValue(static_cast<unsigned char>(1));
    if (objAttackerIndex) {
        const EG::ValueObject<int>* voAttackerIndex = static_cast<const EG::ValueObject<int>*>(objAttackerIndex);
        if (voAttackerIndex) {
            attackerIndex = voAttackerIndex->getDataCopy();
            CCLOG("Received MINION_ATTACK_EVENT with attackerIndex: %d", attackerIndex);
        }
        else {
            CCLOG("Failed to cast attackerIndex in MINION_ATTACK_EVENT.");
            cocosUIListener->writeString(EG::JString(L"Failed to cast attackerIndex in MINION_ATTACK_EVENT."));
            return;
        }
    }
    else {
        CCLOG("attackerIndex not found in MINION_ATTACK_EVENT.");
        cocosUIListener->writeString(EG::JString(L"attackerIndex not found in MINION_ATTACK_EVENT."));
        return;
    }

    // ��ȡ defenderIndex��������������
    const EG::Object* objDefenderIndex = parameters.getValue(static_cast<unsigned char>(2));
    if (objDefenderIndex) {
        const EG::ValueObject<int>* voDefenderIndex = static_cast<const EG::ValueObject<int>*>(objDefenderIndex);
        if (voDefenderIndex) {
            defenderIndex = voDefenderIndex->getDataCopy();
            CCLOG("Received MINION_ATTACK_EVENT with defenderIndex: %d", defenderIndex);
        }
        else {
            CCLOG("Failed to cast defenderIndex in MINION_ATTACK_EVENT.");
            cocosUIListener->writeString(EG::JString(L"Failed to cast defenderIndex in MINION_ATTACK_EVENT."));
            return;
        }
    }
    else {
        CCLOG("defenderIndex not found in MINION_ATTACK_EVENT.");
        cocosUIListener->writeString(EG::JString(L"defenderIndex not found in MINION_ATTACK_EVENT."));
        return;
    }

    cardSprite* attacker = nullptr;
    cardSprite* defender = nullptr;
    if (playerNumber == localPlayerNumber) { // ������ҹ���
        attacker = localMinionCard[attackerIndex];
        if (defenderIndex != -1)
            defender = oppentMinionCard[defenderIndex];
    } else {
        attacker = oppentMinionCard[attackerIndex];
        if (defenderIndex != -1)
            defender = localMinionCard[defenderIndex];
    }

    if (defenderIndex == -1) { // ����Ӣ��
        auto targetplayer = (localPlayerNumber == 1) ? player2 : player1;
        auto localplayer = (localPlayerNumber == 1) ? player1 : player2;
        if (!targetplayer->getDamage(attacker->currentAttack)) {
            endGame(localplayer);
            cocosUIListener->writeString(EG::JString(L"Hero is dead."));
        }
        updatePlayerUI();
        handleMinionAttackHero();
        return;
    }

    attacker->getDamage(defender->currentAttack);
    defender->getDamage(attacker->currentAttack);

    attackmove(playerNumber, attackerIndex, defenderIndex);
    updateCardStats(defender);
    updateCardStats(attacker);

    // ����������
    checkMinionDie(attacker);
    checkMinionDie(defender);

    // ����UI
    updatePlayerUI();

    // ���Ź�����Ч
    audioPlayer("Music/attack.mp3", false);
}

// ���ӿ��Ƶ�ս�� ֻ�ж��ִ�������ʱ����øú���
void BoardScene::add_NewCardToBattlefield(int playerNumber, int cardNumber) {
    // �������ƾ��飬ʹ��������� createWithCard ����
    auto newCard = jsonmanager.find_by_dbfId(cardNumber);
    if (!newCard) {
        CCLOG("Card data not found for cardNumber: %d", cardNumber);
        cocosUIListener->writeString(EG::JString(L"Card data not found for cardNumber: ") +
            EG::JString(std::to_wstring(cardNumber).c_str()));
        return;
    }
    Size desiredSize(200, 250); // ������Ҫ����
    // �������ƾ���
    auto battlefieldCard = cardSprite::createWithCard(newCard, desiredSize); // ʹ��������ķ���
    if (battlefieldCard) {
        battlefieldCard->setTag(cardNumber); // ʹ�� cardNumber ��Ϊ tag

        auto minionCard = std::dynamic_pointer_cast<MinionCard>(newCard);
        if (!minionCard) throw std::runtime_error("newCard is not a minion card");

        battlefieldCard->currentAttack = minionCard.get()->attack;
        battlefieldCard->currentHealth = minionCard.get()->maxhealth;

        //if (playerNumber == localPlayerNumber) {
        //    // ��������Ѵ���Ŀ���
        //    //targetPos = Vec2(PLAYED_AREA_PLAYER1_X + localMinionCard.size() * CARD_SPACING, PLAYED_AREA_Y);
        //    localMinionCard.push_back(battlefieldCard);
        //    updatePlayedCardsPosition();
        //}
        //else {
        //    oppentMinionCard.push_back(battlefieldCard);
        //    updateEnemyCardsPosition();
        //}

        oppentMinionCard.push_back(battlefieldCard);
        updateEnemyCardsPosition();

        // ���ӵ�ս������¼λ��
        this->addChild(battlefieldCard, 50);

        //// ������ʾ����
        //battlefieldCard->runAction(Sequence::create(
        //    FadeIn::create(0.5f),
        //    MoveTo::create(0.5f, targetPos),
        //    nullptr
        //));
    }
    else {
        CCLOG("Failed to create battlefield card sprite for cardNumber: %d", cardNumber);
        cocosUIListener->writeString(EG::JString(L"Failed to create battlefield card sprite for cardNumber: ") +
            EG::JString(std::to_wstring(cardNumber).c_str()));
    }
}

// ���ӿ��Ƶ�ս�� ֻ�б�����Ҵ�������ʱ����øú���
void BoardScene::add_HandCardToBattlefield(cardSprite* minion) {
    if (!minion) {
        CCLOG("Invalid card: minion is null");
        return;
    }

    auto minionCard = std::dynamic_pointer_cast<MinionCard>(minion->card);
    if (!minionCard) { throw std::runtime_error("Card is not a minion card"); return; }

    // ���ÿ��ƵĹ�����������ֵ
    minion->currentAttack = minionCard->attack;
    minion->currentHealth = minionCard->maxhealth;

    localMinionCard.push_back(minion);
    updatePlayedCardsPosition();
}

// �����غϿ�ʼ�¼�
void BoardScene::handle_TurnStart(const EG::Hashtable& parameters) {
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

// ���ӿ��Ƶ�������ң���������ʾ��
void BoardScene::addCardToLocalPlayer(std::shared_ptr<CardBase> card) {
    players::Player* localPlayer = (localPlayerNumber == 1) ? player1 : player2;
    // localPlayer->addCardToHand(card);

    Size desiredSize(120, 180); // ������Ҫ�������Ⱥ͸߶�

    cardSprite* newCard = cardSprite::createWithCard(card, desiredSize);
    addCardStats(newCard);  // Ĭ�����ԣ�Ѫ��5��������1������1
    if (newCard) {
        CCLOG("Successfully created cardSprite for cardNumber: %d", card->dbfId);
        newCard->setTag(card->dbfId); // ʹ�� cardNumber ��Ϊ tag
        Vec2 originalPos(CARD_REGION_X + localPlayerCards.size() * (newCard->getContentSize().width + 30),
            CARD_REGION_Y);
        CCLOG("Setting card position to: (%f, %f)", originalPos.x, originalPos.y);

        // ���ӵ����������򲢼�¼λ��
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

bool BoardScene::checkMinionDie(cardSprite* minion) {
    if (minion->currentHealth <= 0) {
        CCLOG("Releasing attacker with health %d", minion->currentHealth);
        minion->removeFromParentAndCleanup(true);

        // Ȼ��ִ���Ƴ�����
        removeCardWithAnimation(minion);
        return true;
    }
    return false;
}

int BoardScene::get_localMinionIndex(cardSprite* minion) {
    for (size_t i = 0; i < localMinionCard.size(); ++i) {
        if (localMinionCard[i] == minion) return i;
    }
    throw std::runtime_error("Minion not found in localMinionCard");
    return -1;
}

int BoardScene::get_opponentMinionIndex(cardSprite* minion) {
    for (size_t i = 0; i < localMinionCard.size(); ++i) {
        if (oppentMinionCard[i] == minion) return i;
    }
    throw std::runtime_error("Minion not found in opponentMinionCard");
    return -1;
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
    // �������ƾ���
    for (auto& card : localPlayerCards) {
        if (card) {
            card->removeFromParent();
        }
    }
    localPlayerCards.clear();

    for (auto& card : localMinionCard) {
        if (card) {
            card->removeFromParent();
        }
    }
    localMinionCard.clear();

    for (auto& card : oppentMinionCard) {
        if (card) {
            card->removeFromParent();
        }
    }
    oppentMinionCard.clear();

    // ��������λ�ü�¼
    cardOriginalPositions.clear();

    player1 = nullptr;
    player2 = nullptr;
    CCLOG("BoardScene destroyed and all card references cleared.");
}


///Ŀǰ���� UI�����䣬ԭ���Ŀ������Ȼ���ڣ�ͬʱû����ȷ��ʵ�ֶ��ֿ��Ƶ���ʾ