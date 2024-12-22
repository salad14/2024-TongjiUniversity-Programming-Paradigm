/****************************************************************
 * ���̳�����ʵ��
 * Author:  Lee
 * ע����ʱ����ui���濪�����ԣ�δ���������߼�
 ****************************************************************/

#include "BoardScene.h"
#include "MainScene.h"
#include "proj.win32/Alluse.h"
#include "proj.win32/audioPlayer.h"
#include <random>
#include <windows.h>  // ���� Sleep ����
using namespace std;
USING_NS_CC;

Scene* BoardScene::createScene() {
    return BoardScene::create();
}

bool BoardScene::init() {
    if (!Scene::init()) {
        return false;
    }
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();
    // ���ñ���
    auto title_sprite = Sprite::create("board.png");
    if (title_sprite == nullptr) {
        printf("Error loading board.png\n");
        return false;
    }
    title_sprite->setContentSize(visibleSize);
    title_sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    title_sprite->setTag(0);  // �������
    // ������������
    // title_sprite->setName("background");
    this->addChild(title_sprite, 0);
    // �������ذ�ť
    auto cancel = MenuItemImage::create(
        "button/cancel.png",
        "button/cancelSelected.png",
        CC_CALLBACK_1(BoardScene::cancelCallback, this)
    );
    cancel->setPosition(Vec2(1940, 20));
    auto menu = Menu::create(cancel, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
    // ��������������
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BoardScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BoardScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BoardScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    // ��������ƶ�������
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = CC_CALLBACK_1(BoardScene::onMouseMove, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
    // ��ʼ������
    selectedCard = nullptr;
    hoveredCard = nullptr;
    attackingCard = nullptr;
    attackIndicator = nullptr;
    // ������Ϸ�������
    createDropArea();
    initPlayers();
    createPlayerUI();
    updatePlayerUI();
    initDecks();
    //initEnemyCards();
    // ��ʼ����Ϸ������ر���
    isGameOver = false;
    gameOverLabel = nullptr;

    return true;
}
        
//// �����з���ӣ��������ԣ���ʼ����4��������ڳ��ϣ�
//void BoardScene::initEnemyCards() {
//    // ��ʼ��4�ŵз���ӿ���
//    for (int i = 0; i < 4; i++) {
//        auto card = Sprite::create("card1.png");
//        card->setScale(0.8f);
//
//        // ������Ա�ǩ
//        addCardStats(card, 3 + (i % 2) + 1, (i % 2) + 1, (i % 2) + 1, "spell");  
//
//        enemyPlayedCards.push_back(card);
//        this->addChild(card);
//    }
//    updateEnemyCardsPosition();
//}
            
// ��ӿ��Ƶ����Ա�ǩ
void BoardScene::addCardStats(Sprite* card, int health, int attack, int cost,string type) {
    // �洢��������
    cardStats[card] = { health, attack, cost ,type};

    // ������������Ա�ǩ
    auto healthLabel = Label::createWithTTF(std::to_string(health), "fonts/arial.ttf", 24);
    healthLabel->setPosition(Vec2(card->getContentSize().width - 20, 20));
    healthLabel->setName("healthLabel");
    healthLabel->enableOutline(Color4B::BLACK, 2);
    card->addChild(healthLabel);

    auto attackLabel = Label::createWithTTF(std::to_string(attack), "fonts/arial.ttf", 24);
    attackLabel->setPosition(Vec2(20, 20));
    attackLabel->setName("attackLabel");
    attackLabel->enableOutline(Color4B::BLACK, 2);
    card->addChild(attackLabel);

    auto costLabel = Label::createWithTTF(std::to_string(cost), "fonts/arial.ttf", 24);
    costLabel->setPosition(Vec2(20, card->getContentSize().height - 20));
    costLabel->setName("costLabel");
    costLabel->enableOutline(Color4B::BLACK, 2);
    card->addChild(costLabel);

    auto typeLabel = Label::createWithTTF(type, "fonts/arial.ttf", 24);
    typeLabel->setPosition(Vec2(card->getContentSize().width/2, card->getContentSize().height/2));
    typeLabel->setName("typeLabel");
    typeLabel->enableOutline(Color4B::BLACK, 10);
    card->addChild(typeLabel);
}
  
// ���¿����ϵ����Ա�ǩ
void BoardScene::updateCardStats(Sprite* card)
{
    if (!card) return;
    // ��鿨���Ƿ������ cardStats ��
    auto it = cardStats.find(card);
    if (it == cardStats.end()) {
        return;
    }
    // ʹ�õ�����������ֱ�ӷ���
    const auto& stats = it->second;
    // ��ȡ��ǩ
    auto healthLabel = dynamic_cast<Label*>(card->getChildByName("healthLabel"));
    auto attackLabel = dynamic_cast<Label*>(card->getChildByName("attackLabel"));
    auto costLabel = dynamic_cast<Label*>(card->getChildByName("costLabel"));
    // ���±�ǩ
    if (healthLabel && stats.health > 0) {
        healthLabel->setString(std::to_string(stats.health));
    }
    if (attackLabel) {
        attackLabel->setString(std::to_string(stats.attack));
    }
    if (costLabel) {
        costLabel->setString(std::to_string(stats.cost));
    }
}

// ������������
void BoardScene::createDropArea() {
    dropArea = DrawNode::create();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 center = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);
    Vec2 vertices[] = {
       Vec2(center.x - PUTOUT_CARD_REGION_HALF_X, center.y - PUTOUT_CARD_REGION_HALF_Y),
       Vec2(center.x + PUTOUT_CARD_REGION_HALF_X, center.y - PUTOUT_CARD_REGION_HALF_Y),
       Vec2(center.x + PUTOUT_CARD_REGION_HALF_X, center.y + PUTOUT_CARD_REGION_HALF_Y),
       Vec2(center.x - PUTOUT_CARD_REGION_HALF_X, center.y + PUTOUT_CARD_REGION_HALF_Y)
    };
    Color4F fillColor(0.5f, 0.5f, 0.5f, 0.3f);
    Color4F borderColor(1.0f, 1.0f, 1.0f, 0.8f);
    dropArea->drawPolygon(vertices, 4, fillColor, 2, borderColor);
    this->addChild(dropArea, 1);
}
    
// ��鿨���Ƿ��ڳ�������
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
    
// ����������ͨ�������ң������ҹ��ã�
void BoardScene::attackmove(int attackerIndex, int defenderIndex) {
    if (isPlayer1Turn)
    {
        if (attackerIndex < 0 || attackerIndex >= playedCards.size() ||
            defenderIndex < 0 || defenderIndex >= enemyPlayedCards.size()) {
            return;
        }
        auto attacker = playedCards[attackerIndex];
        auto defender = enemyPlayedCards[defenderIndex];
        // ���湥���ߵ�ԭʼλ��
        Vec2 originalPos = attacker->getPosition();
        // ��ȡĿ��λ��
        Vec2 targetPos = defender->getPosition();
        // ����������������
        attacker->runAction(Sequence::create(
            // �����ƶ���Ŀ��λ��
            EaseIn::create(MoveTo::create(0.2f, targetPos), 2.0f),
            // ���һ���̵ܶ�ͣ��
            DelayTime::create(0.1f),
            // ����ԭʼλ��
            EaseOut::create(MoveTo::create(0.2f, originalPos), 2.0f),
            nullptr
        ));
        // �������һ���򵥵��ܻ�Ч��
        defender->runAction(Sequence::create(
            // �ζ�Ч��
            RotateBy::create(0.1f, 10),
            RotateBy::create(0.1f, -20),
            RotateBy::create(0.1f, 10),
            nullptr
        ));
    }
    else
    {
        if (attackerIndex < 0 || attackerIndex >= playedCards.size() ||
            defenderIndex < 0 || defenderIndex >= enemyPlayedCards.size()) {
            return;
        }
        auto attacker = enemyPlayedCards[attackerIndex];
        auto defender = playedCards[defenderIndex];
        // ���湥���ߵ�ԭʼλ��
        Vec2 originalPos = attacker->getPosition();
        // ��ȡĿ��λ��
        Vec2 targetPos = defender->getPosition();
        // ����������������
        attacker->runAction(Sequence::create(
            // �����ƶ���Ŀ��λ��
            EaseIn::create(MoveTo::create(1.0f, targetPos), 2.0f),
            // ���һ���̵ܶ�ͣ��
            DelayTime::create(0.3f),
            // ����ԭʼλ��
            EaseOut::create(MoveTo::create(1.0f, originalPos), 2.0f),
            nullptr
        ));
        // �������һ���򵥵��ܻ�Ч��
        defender->runAction(Sequence::create(
            // �ζ�Ч��
            RotateBy::create(0.1f, 10),
            RotateBy::create(0.1f, -20),
            RotateBy::create(0.1f, 10),
            nullptr
        ));
    }
}

// �����ƹ�������
//Ч����
//1.��2028 * 1280�Ĵ��ڵ׶��м䷢��һ������ͼ�꣨�ļ�·��Ϊ��ImageElements / spell.png�����ƶ�����Ļ���ϵ�Target�ϣ���ʾ����
//2. * Sprite Target ȱʡΪNULL��ΪNULLʱ�ӵ׶��м��ƶ�����ͼ�굽�����м�
void BoardScene::spellmove(Sprite* Target) {
    // ��������ͼ�꾫��
    auto spell = Sprite::create("ImageElements/spell.png");
    Vec2 targetPos;
    if (isPlayer1Turn)
    {
        // ���ó�ʼλ�ã��׶��м䣩
        spell->setPosition(Vec2(2028 / 2, 0));
        this->addChild(spell);
        if (Target == NULL) {
            // ���û��Ŀ�꣬�ƶ��������м�
            targetPos = Vec2(2028 / 2, 1280);

        }
        else {
            // �����Ŀ�꣬�ƶ���Ŀ��λ��
            targetPos = Target->getPosition();
        }
    }
    else
    {
        // ���ó�ʼλ�ã������м䣩
        spell->setPosition(Vec2(2028 / 2, 1280));
        this->addChild(spell);
        if (Target == NULL) {
            // ���û��Ŀ�꣬�ƶ����׶��м�
            targetPos = Vec2(2028 / 2, 0);

        }
        else {
            // �����Ŀ�꣬�ƶ���Ŀ��λ��
            targetPos = Target->getPosition();
        }
    }
    // �����ƶ�����
    auto moveTo = MoveTo::create(0.8f, targetPos);

    // ������������
    auto fadeOut = FadeOut::create(0.2f);

    // �����������У��ƶ��󵭳����Ƴ���
    auto sequence = Sequence::create(
        moveTo,
        fadeOut,
        RemoveSelf::create(),
        nullptr
    );
    // ������Ч
    audioPlayer("Music/spell.mp3", false);
    // ִ�ж���
    spell->runAction(sequence);
}
// ������Ӷ���ӵĹ���
void BoardScene::handleAttack(Sprite* attacker, Sprite* defender) {
    if (isPlayer1Turn)
    {
        hasAttackedThisTurn = true;  // ����Ѿ�����ӹ���
        // �ҵ������ߺͷ����ߵ�����
        int attackerIndex = -1;
        int defenderIndex = -1;
        // ���ҹ���������
        for (size_t i = 0; i < playedCards.size(); i++) {
            if (playedCards[i] == attacker) {
                attackerIndex = i;
                break;
            }
        }
        // ���ҷ���������
        for (size_t i = 0; i < enemyPlayedCards.size(); i++) {
            if (enemyPlayedCards[i] == defender) {
                defenderIndex = i;
                break;
            }
        }
        // ���Ź�������
        if (attackerIndex != -1 && defenderIndex != -1) {
            attackmove(attackerIndex, defenderIndex);
           
        }
        // �ȸ�����Ҫ�����ݣ�����������ʿ��ܱ�ɾ��������
        int attackPower = 0;
        // ��ȡ������
        auto attackerIt = cardStats.find(attacker);
        if (attackerIt != cardStats.end()) {
            attackPower = attackerIt->second.attack;
        }
        else {
            return;
        }
        // ��ȡ�����·����ߵ�����ֵ
        auto defenderIt = cardStats.find(defender);
        if (defenderIt != cardStats.end()) {
            defenderIt->second.health -= attackPower;

            // �ȸ�����ʾ
            updateCardStats(defender);

            // �������ֵС�ڵ���0���Ƴ�����
            if (defenderIt->second.health <= 0) {
                // �ȴ� cardStats ���Ƴ�
                cardStats.erase(defender);
                // Ȼ��ִ���Ƴ�����
                removeCardWithAnimation(defender);
            }
        }
        // ���Ź�����Ч
        audioPlayer("Music/attack.mp3", false);
    }
    else
    {
        // �ҵ������ߺͷ����ߵ�����
        int attackerIndex = -1;
        int defenderIndex = -1;
        // ���ҹ���������
        for (size_t i = 0; i < enemyPlayedCards.size(); i++) {
            if (enemyPlayedCards[i] == attacker) {
                attackerIndex = i;
                break;
            }
        }
        // ���ҷ���������
        for (size_t i = 0; i < playedCards.size(); i++) {
            if (playedCards[i] == defender) {
                defenderIndex = i;
                break;
            }
        }
        // ���Ź�������
        if (attackerIndex != -1 && defenderIndex != -1) {
            attackmove(attackerIndex, defenderIndex);
            //Sleep(2000);
        }
        // �ȸ�����Ҫ�����ݣ�����������ʿ��ܱ�ɾ��������
        int attackPower = 0;
        // ��ȡ������
        auto attackerIt = cardStats.find(attacker);
        if (attackerIt != cardStats.end()) {
            attackPower = attackerIt->second.attack;
        }
        else {
            return;
        }
        // ��ȡ�����·����ߵ�����ֵ
        auto defenderIt = cardStats.find(defender);
        if (defenderIt != cardStats.end()) {
            defenderIt->second.health -= attackPower;

            // �ȸ�����ʾ
            updateCardStats(defender);

            // �������ֵС�ڵ���0���Ƴ�����
            if (defenderIt->second.health <= 0) {
                // �ȴ� cardStats ���Ƴ�
                cardStats.erase(defender);
                // Ȼ��ִ���Ƴ�����
                removeCardWithAnimation(defender);
            }
        }
        // ���Ź�����Ч
        audioPlayer("Music/attack.mp3", false);
    }
    // ����UI
    updatePlayerUI();
    checkGameOver();
}
 
// �����Ӣ�۵Ĺ���
void BoardScene::handleAttackToHero() {
    if (!attackingCard || !isPlayer1Turn) return;
    hasAttackedThisTurn = true;  // ����Ѿ�����ӹ���
    // ��鿨���Ƿ������ cardStats ��
    if (cardStats.find(attackingCard) == cardStats.end()) {
        return;
    }
    // ��ȡ������ӵĹ�����
    auto& attackerStats = cardStats[attackingCard];
    // �۳��Է�Ӣ������ֵ
    player2->health -= attackerStats.attack;
    // ����UI
    updatePlayerUI();
    checkGameOver();
    // ���Ź�����Ч
    audioPlayer("Music/attack.mp3", false);
    //����ʵ��һ��������Ч��
    // ��ȡ�������飨ͨ��tag�����֣�
    auto background = this->getChildByTag(0);  // ֮ǰ��ӱ���ʱ���õ�tagΪ0
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
void BoardScene::removeCardWithAnimation(Sprite* card) {
    if (!card) return;
    // �ȴ� cardStats ���Ƴ�
    cardStats.erase(card);

    // ������ʧ����
    card->runAction(Sequence::create(
        Spawn::create(
            FadeOut::create(0.5f),
            ScaleTo::create(0.5f, 0.1f),
            nullptr
        ),
        CallFunc::create([this, card]() {
            // ���������Ƴ�
            auto it = std::find(enemyPlayedCards.begin(), enemyPlayedCards.end(), card);
            if (it != enemyPlayedCards.end()) {
                enemyPlayedCards.erase(it);
            }
            it = std::find(playedCards.begin(), playedCards.end(), card);
            if (it != playedCards.end()) {
                playedCards.erase(it);
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
  
// ���µ��˵����λ��
void BoardScene::updateEnemyCardsPosition() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height * 0.6f);
    float cardWidth = 100;
    float spacing = 20;

    float startX = center.x - (enemyPlayedCards.size() * (cardWidth + spacing) - spacing) / 2;

    for (size_t i = 0; i < enemyPlayedCards.size(); i++) {
        Sprite* card = enemyPlayedCards[i];
        Vec2 targetPos = Vec2(startX + i * (cardWidth + spacing), center.y);
        card->runAction(EaseBackOut::create(MoveTo::create(0.3f, targetPos)));
    }
}

// �����ʼ����
bool BoardScene::onTouchBegan(Touch* touch, Event* event) {

    if (isPlayer1Turn)
    {
        Vec2 touchLocation = touch->getLocation();

        // ����Ѿ�ѡ���˹�����ӣ�����Ƿ�������ЧĿ��
        if (attackingCard) {
            // ����Ƿ����˵з���ӻ�з�Ӣ������
            bool targetFound = false;

            // ���з����
            for (auto enemyCard : enemyPlayedCards) {
                if (enemyCard->getBoundingBox().containsPoint(touchLocation)) {
                    handleAttack(attackingCard, enemyCard);
                    targetFound = true;
                    break;
                }
            }

            // ���з�Ӣ������
            if (!targetFound && touchLocation.y > Director::getInstance()->getVisibleSize().height * 0.7f) {
                handleAttackToHero();
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
        for (auto card : playedCards) {
            if (card->getBoundingBox().containsPoint(touchLocation)) {
                // ����Ƿ��Ѿ�������
                if (hasAttackedThisTurn) {
                    // ���������ʾЧ��
                    return false;
                }
                attackingCard = card;
                createAttackIndicator(card->getPosition());
                return true;
            }
        }

        // �����������Ŀ���
        for (auto it = dragCards.rbegin(); it != dragCards.rend(); ++it) {
            Sprite* sprite = *it;
            Vec2 locationInNode = sprite->convertToNodeSpace(touchLocation);
            Size s = sprite->getContentSize();
            Rect rect = Rect(0, 0, s.width, s.height);
            if (rect.containsPoint(locationInNode)) {
                selectedCard = sprite;
                scaleSprite(sprite, 1.0f);
                return true;
            }
        }
        selectedCard = nullptr;
        return false;
    }
}
    
// ����϶�����
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
       
// �����������
void BoardScene::onTouchEnded(Touch* touch, Event* event) {
    if (selectedCard) {
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);
        Vec2 finalPos = selectedCard->getPosition();
        bool inDropArea =
            abs(finalPos.x - center.x) <= PUTOUT_CARD_REGION_HALF_X &&
            abs(finalPos.y - center.y) <= PUTOUT_CARD_REGION_HALF_Y;
        Sprite* cardToHandle = selectedCard;
        selectedCard = nullptr;
        if (inDropArea) {
            Player* currentPlayer = isPlayer1Turn ? player1 : player2;
            int cardCost = cardStats[cardToHandle].cost;
            if (currentPlayer->money >= cardCost) {
                currentPlayer->money -= cardCost;
                removeCard(cardToHandle);
                updatePlayerUI();
                checkGameOver();
            }
            else {
                returnCardToHand(cardToHandle);
            }
        }
        else {
            returnCardToHand(cardToHandle);
        }
    }
}
            
// �����ƷŻ�����
void BoardScene::returnCardToHand(Sprite* card) {
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
          
// ����ƶ�����
void BoardScene::onMouseMove(Event* event) {
    EventMouse* mouseEvent = dynamic_cast<EventMouse*>(event);
    Vec2 mousePos = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());
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
    if (hoveredCard != newHoveredSprite) {
        if (hoveredCard && hoveredCard != selectedCard) {
            scaleSprite(hoveredCard, 1.0f);
        }
        if (newHoveredSprite && newHoveredSprite != selectedCard) {
            scaleSprite(newHoveredSprite, 1.5f);
        }
        hoveredCard = newHoveredSprite;
    }
}

// ����Sprite
void BoardScene::scaleSprite(Sprite* sprite, float scale) {
    sprite->runAction(ScaleTo::create(0.1f, scale));
}
    
// ����
void BoardScene::removeCard(Sprite* sprite) {
    if (!sprite) return;
    if (sprite == hoveredCard) {
        hoveredCard = nullptr;
    }
    auto iter = find(dragCards.begin(), dragCards.end(), sprite);
    if (iter != dragCards.end()) {
        size_t removedIndex = distance(dragCards.begin(), iter);
        dragCards.erase(iter);
        cardOriginalPositions.erase(sprite);

        //��������������
        if (cardStats[sprite].type == "partner")
        {
            // ��ӵ�����
            playedCards.push_back(sprite);
            updatePlayedCardsPosition();
        }
        
        //����������Ƿ���
        if (cardStats[sprite].type == "spell")
        {
            spellmove(NULL);
            // �۳��Է�Ӣ������ֵ
            player2->health -= cardStats[sprite].attack;
            // ����UI
            updatePlayerUI();
            checkGameOver();
            sprite->removeFromParent();
        }
        
        // ������Ч
        audioPlayer("music/putcard.mp3", false);
        // ����ʣ������λ��
        for (size_t i = removedIndex; i < dragCards.size(); i++) {
            Sprite* card = dragCards[i];
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

    float startX = center.x - (playedCards.size() * (cardWidth + spacing) - spacing) / 2;

    for (size_t i = 0; i < playedCards.size(); i++) {
        Sprite* card = playedCards[i];
        Vec2 targetPos = Vec2(startX + i * (cardWidth + spacing), center.y);
        card->runAction(EaseBackOut::create(MoveTo::create(0.3f, targetPos)));
    }
}
         
// ��ʼ�������Ϣ
void BoardScene::initPlayers() {
    player1 = new Player("Player1");
    player2 = new Player("Player2");
    isPlayer1Turn = true;
    player1->setPlayerCards();
    player2->setPlayerCards();
}
     
// �������UI
void BoardScene::createPlayerUI() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    // �������������Ϣ��ʾ
    player1Health = Label::createWithTTF("HP: 30", "fonts/arial.ttf", 24);
    player1Health->setPosition(Vec2(800, 220));
    player1Health->enableOutline(Color4B::BLACK, 2);
    this->addChild(player1Health);
    player1Mana = Label::createWithTTF("Mana: 1/1", "fonts/arial.ttf", 24);
    player1Mana->setPosition(Vec2(800, 280));
    player1Mana->enableOutline(Color4B::BLACK, 2);
    this->addChild(player1Mana);
    // �����Է������Ϣ��ʾ
    player2Health = Label::createWithTTF("HP: 30", "fonts/arial.ttf", 24);
    player2Health->setPosition(Vec2(800, visibleSize.height - 220));
    player2Health->enableOutline(Color4B::BLACK, 2);
    this->addChild(player2Health);
    player2Mana = Label::createWithTTF("Mana: 1/1", "fonts/arial.ttf", 24);
    player2Mana->setPosition(Vec2(800, visibleSize.height - 280));
    player2Mana->enableOutline(Color4B::BLACK, 2);
    this->addChild(player2Mana);
    // �غ�ָʾ��
    turnIndicator = Label::createWithTTF("Your Turn", "fonts/arial.ttf", 32);
    turnIndicator->setPosition(Vec2(1880, 600));
    turnIndicator->enableOutline(Color4B::BLACK, 2);
    this->addChild(turnIndicator);
    // �غϽ�����ť
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
    
// �غ��л�
void BoardScene::switchTurn() {

    isPlayer1Turn = !isPlayer1Turn;
    hasAttackedThisTurn = false;  // ���ù������
    // ���ص����Ч
    audioPlayer("Music/ClickSoundEffect.mp3", false);

    // �л��غ�ʱ���߼�
    Player* currentPlayer = isPlayer1Turn ? player1 : player2;
    currentPlayer->maxmoney = std::min(currentPlayer->maxmoney + 1, 10);
    currentPlayer->money = currentPlayer->maxmoney;
    // ����UI
    updatePlayerUI();
    checkGameOver();
    // �غ��л�����
    turnIndicator->runAction(Sequence::create(
        FadeOut::create(0.2f),
        CallFunc::create([this]() {
            updatePlayerUI();
            }),
        FadeIn::create(0.2f),
        nullptr
    ));

    AIplay();

    isPlayer1Turn = !isPlayer1Turn;
    // �л��غ�ʱ���߼�
    currentPlayer = isPlayer1Turn ? player1 : player2;
    currentPlayer->maxmoney = std::min(currentPlayer->maxmoney + 1, 10);
    currentPlayer->money = currentPlayer->maxmoney;
    // ����UI
    updatePlayerUI();
    checkGameOver();
    // �غ��л�����
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
    for (int i = 0; i < 30; i++) {
        // �������1���ƶ�
        player1->playerCards.at(i)->setPosition(Vec2(-i * 2.0f, -i * 2.0f));
        deckNode1->addChild(player1->playerCards.at(i));

        // �������2���ƶ�
        player2->playerCards.at(i)->setPosition(Vec2(-i * 2.0f, -i * 2.0f));
        deckNode2->addChild(player2->playerCards.at(i));
    }
}
         
// ���ƶ��г�һ����
void BoardScene::drawCard() {
    if (isPlayer1Turn && !player1->playerCards.empty()) {
        // ���ƶѳ�һ����
        auto card = player1->playerCards.back();
        player1->playerCards.pop_back();
        // �����µĿɲ�������
        auto texture = card->getTexture();
        auto newCard = Sprite::createWithTexture(texture);
        newCard->setPosition(Vec2(1950, 500)); // �ƶ�λ��
        newCard->setScale(0.5f);
        // ����Ŀ��λ��
        Vec2 originalPos(CARD_REGION_X + dragCards.size() * (newCard->getContentSize().width + 30),
            CARD_REGION_Y);
        // ��ӵ����������򲢼�¼λ��
        this->addChild(newCard);
        cardOriginalPositions[newCard] = originalPos;
        dragCards.push_back(newCard);

        // ������ɿ�������
        int health = 1 + rand() % 7;
        int damage = 1 + rand() % 7;
        int cost = 1 + rand() % 7;
        string type = rand() % 2 ? "spell" : "partner";
        // ��ӿ�������
        addCardStats(newCard, health, damage, cost, type);

        // ���ƶ�������
        newCard->runAction(Sequence::create(
            EaseOut::create(MoveBy::create(0.2f, Vec2(0, 50)), 2.0f),
            Spawn::create(
                EaseInOut::create(MoveTo::create(0.5f, originalPos), 2.0f),
                EaseInOut::create(ScaleTo::create(0.5f, 1.0f), 2.0f),
                RotateBy::create(0.5f, 360),
                nullptr
            ),
            EaseElasticOut::create(ScaleTo::create(0.3f, 1.0f)),
            nullptr
        ));
        // ���ų�����Ч
        audioPlayer("Music/drawcard.mp3", false);
        // �����ƶ���ʾ
        deckNode1->removeChild(card);
    }
}

void BoardScene::AIplay() 
{
    // ���ƶѳ�һ����
    auto card = player2->playerCards.back();
    player2->playerCards.pop_back();
    auto texture = card->getTexture();
    card->removeFromParent();

    auto newCard = Sprite::createWithTexture(texture);
    int health = 1 + rand() % 5;  // 1-5������ֵ
    int damage = 1 + rand() % 4;  // 1-4�Ĺ�����
    int cost = 1 + rand() % 3;    // 1-3�ķ���
    string type = (rand() % 100 < 50) ? "partner" : "spell";  // 50%�����������

    addCardStats(newCard, health, damage, cost, type);
    if (type == "partner") {

        // ���ų�����Ч
        audioPlayer("Music/putcard.mp3", false);
        newCard->setScale(0.8f);
        // ��ӵ�����
        enemyPlayedCards.push_back(newCard);
        this->addChild(newCard);
        updateEnemyCardsPosition();
        
    }
    else {
        // ���ų�����Ч
        audioPlayer("Music/putcard.mp3", false);
        newCard->setScale(0.8f);
        // ��ӵ�����
        enemyPlayedCards.push_back(newCard);
        this->addChild(newCard);
        updateEnemyCardsPosition();

        enemyPlayedCards.pop_back();
        // ����һ��ʱ������ҿ���
        // ����Ŀ��Ŵ����С�ı���
        float scaleUpFactor = 2.0f;   // �Ŵ���
        float scaleDownFactor = 0.0f; // ��С����ʧ�����մ�СΪ 0
        // �����Ŵ�����3�����ڴӵ�ǰ��С�Ŵ� 2 ��
        auto scaleUpAction = cocos2d::ScaleTo::create(1.5f, scaleUpFactor); // �Ŵ� 1.5 ��
        // ������С������2�����ڴӷŴ���״̬��С�� 0
        auto scaleDownAction = cocos2d::ScaleTo::create(1.5f, scaleDownFactor); // ��С 1.5 ��
        // ������ʧ������3������͸���ȱ�Ϊ 0
        auto fadeOutAction = cocos2d::FadeOut::create(1.5f);  // ͸���ȱ仯���� 1.5 ��
        // �ڶ����������Ƴ�����
        newCard->runAction(cocos2d::Sequence::create(
            scaleUpAction,
            scaleDownAction,
            fadeOutAction,
            cocos2d::CallFunc::create([newCard]() {
                newCard->removeFromParent();  // ������ɺ��Ƴ�����
                }), nullptr
        ));
        updateEnemyCardsPosition();
       
        // ʹ�÷�����ֱ������˺�
        spellmove(nullptr);
        player1->health -= damage;
        audioPlayer("Music/spell.mp3", false);
    }

    // AI�������
    if (!enemyPlayedCards.empty()) {
        auto attackerCard = enemyPlayedCards.at(rand() % enemyPlayedCards.size());//�����ѡһ��ai�����
        // 80%���ʷ�������
        if (rand() % 100 < 80) {
            if (!playedCards.empty()) {
                // ���ѡ��һ�������ӹ���
                int targetIndex = rand() % playedCards.size();
                handleAttack(attackerCard, playedCards[targetIndex]);
            }
            else {
                // ֱ�ӹ������Ӣ��
                player1->health -= cardStats[attackerCard].attack;
                audioPlayer("Music/attack.mp3", false);
                //Sleep(1000);

            }
        }
        
    }
    updatePlayerUI();
    checkGameOver();
}

// �����Ϸ�������
void BoardScene::checkGameOver() {
    if (isGameOver) return;

    if (player1->health <= 0 || player2->health <= 0) {
        isGameOver = true;

        // ������Ϸ������ǩ
        gameOverLabel = Label::createWithTTF(
            player1->health <= 0 ? "You Lose" : "You Win",
            "fonts/arial.ttf",
            120
        );

        auto visibleSize = Director::getInstance()->getVisibleSize();
        gameOverLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        gameOverLabel->enableOutline(Color4B::BLACK, 4);
        this->addChild(gameOverLabel, 100);

        // ��Ӷ���Ч��
        gameOverLabel->setScale(0.1f);
        gameOverLabel->runAction(Sequence::create(
            EaseElasticOut::create(ScaleTo::create(1.0f, 1.0f)),
            nullptr
        ));
    }
}
// ȡ����ť�ص�
void BoardScene::cancelCallback(Ref* pSender) {
    // ���ص����Ч
    audioPlayer("Music/ClickSoundEffect.mp3", false);
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene()));
}

