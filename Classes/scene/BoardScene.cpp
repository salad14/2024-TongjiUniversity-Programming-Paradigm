/****************************************************************
 * 棋盘场景的实现
 * Author:  Lee
 * 注：暂时用于ui界面开发测试，未加入联机逻辑
 ****************************************************************/

#include "BoardScene.h"
#include "MainScene.h"
#include "proj.win32/Alluse.h"
#include "proj.win32/audioPlayer.h"
#include <random>
#include <windows.h>  // 包含 Sleep 函数
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
    // 设置背景
    auto title_sprite = Sprite::create("board.png");
    if (title_sprite == nullptr) {
        printf("Error loading board.png\n");
        return false;
    }
    title_sprite->setContentSize(visibleSize);
    title_sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    title_sprite->setTag(0);  // 添加这行
    // 或者设置名字
    // title_sprite->setName("background");
    this->addChild(title_sprite, 0);
    // 创建返回按钮
    auto cancel = MenuItemImage::create(
        "button/cancel.png",
        "button/cancelSelected.png",
        CC_CALLBACK_1(BoardScene::cancelCallback, this)
    );
    cancel->setPosition(Vec2(1940, 20));
    auto menu = Menu::create(cancel, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
    // 创建触摸监听器
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BoardScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BoardScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BoardScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    // 创建鼠标移动监听器
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = CC_CALLBACK_1(BoardScene::onMouseMove, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
    // 初始化变量
    selectedCard = nullptr;
    hoveredCard = nullptr;
    attackingCard = nullptr;
    attackIndicator = nullptr;
    // 创建游戏基础组件
    createDropArea();
    initPlayers();
    createPlayerUI();
    updatePlayerUI();
    initDecks();
    //initEnemyCards();
    // 初始化游戏结束相关变量
    isGameOver = false;
    gameOverLabel = nullptr;

    return true;
}
        
//// 创建敌方随从（仅供测试，初始化了4张随从牌在场上）
//void BoardScene::initEnemyCards() {
//    // 初始化4张敌方随从卡牌
//    for (int i = 0; i < 4; i++) {
//        auto card = Sprite::create("card1.png");
//        card->setScale(0.8f);
//
//        // 添加属性标签
//        addCardStats(card, 3 + (i % 2) + 1, (i % 2) + 1, (i % 2) + 1, "spell");  
//
//        enemyPlayedCards.push_back(card);
//        this->addChild(card);
//    }
//    updateEnemyCardsPosition();
//}
            
// 添加卡牌的属性标签
void BoardScene::addCardStats(Sprite* card, int health, int attack, int cost,string type) {
    // 存储卡牌属性
    cardStats[card] = { health, attack, cost ,type};

    // 创建并添加属性标签
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
  
// 更新卡牌上的属性标签
void BoardScene::updateCardStats(Sprite* card)
{
    if (!card) return;
    // 检查卡牌是否存在于 cardStats 中
    auto it = cardStats.find(card);
    if (it == cardStats.end()) {
        return;
    }
    // 使用迭代器而不是直接访问
    const auto& stats = it->second;
    // 获取标签
    auto healthLabel = dynamic_cast<Label*>(card->getChildByName("healthLabel"));
    auto attackLabel = dynamic_cast<Label*>(card->getChildByName("attackLabel"));
    auto costLabel = dynamic_cast<Label*>(card->getChildByName("costLabel"));
    // 更新标签
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

// 创建出牌区域
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
    
// 检查卡牌是否在出牌区域
void BoardScene::checkDropArea() {
    if (!selectedCard)
        return;

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    Vec2 spritePos = selectedCard->getPosition();

    // 检查是否在投放区域内
    bool inDropArea =
        abs(spritePos.x - center.x) <= PUTOUT_CARD_REGION_HALF_X &&
        abs(spritePos.y - center.y) <= PUTOUT_CARD_REGION_HALF_Y;
}

// 瞄准 选择攻击对象
void BoardScene::createAttackIndicator(const Vec2& startPos) {
    if (attackIndicator) {
        attackIndicator->removeFromParent();
    }

    attackIndicator = DrawNode::create();
    this->addChild(attackIndicator, 100);

    // 绘制红色瞄准圈
    attackIndicator->drawCircle(startPos, 30, 0, 360, false, Color4F(1, 0, 0, 0.8f));
}
    
// 攻击动画（通过索引找）（敌我公用）
void BoardScene::attackmove(int attackerIndex, int defenderIndex) {
    if (isPlayer1Turn)
    {
        if (attackerIndex < 0 || attackerIndex >= playedCards.size() ||
            defenderIndex < 0 || defenderIndex >= enemyPlayedCards.size()) {
            return;
        }
        auto attacker = playedCards[attackerIndex];
        auto defender = enemyPlayedCards[defenderIndex];
        // 保存攻击者的原始位置
        Vec2 originalPos = attacker->getPosition();
        // 获取目标位置
        Vec2 targetPos = defender->getPosition();
        // 创建攻击动画序列
        attacker->runAction(Sequence::create(
            // 快速移动到目标位置
            EaseIn::create(MoveTo::create(0.2f, targetPos), 2.0f),
            // 添加一个很短的停顿
            DelayTime::create(0.1f),
            // 返回原始位置
            EaseOut::create(MoveTo::create(0.2f, originalPos), 2.0f),
            nullptr
        ));
        // 可以添加一个简单的受击效果
        defender->runAction(Sequence::create(
            // 晃动效果
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
        // 保存攻击者的原始位置
        Vec2 originalPos = attacker->getPosition();
        // 获取目标位置
        Vec2 targetPos = defender->getPosition();
        // 创建攻击动画序列
        attacker->runAction(Sequence::create(
            // 快速移动到目标位置
            EaseIn::create(MoveTo::create(1.0f, targetPos), 2.0f),
            // 添加一个很短的停顿
            DelayTime::create(0.3f),
            // 返回原始位置
            EaseOut::create(MoveTo::create(1.0f, originalPos), 2.0f),
            nullptr
        ));
        // 可以添加一个简单的受击效果
        defender->runAction(Sequence::create(
            // 晃动效果
            RotateBy::create(0.1f, 10),
            RotateBy::create(0.1f, -20),
            RotateBy::create(0.1f, 10),
            nullptr
        ));
    }
}

// 法术牌攻击动画
//效果：
//1.从2028 * 1280的窗口底端中间发出一个火焰图标（文件路径为“ImageElements / spell.png”）移动到屏幕中上的Target上，表示攻击
//2. * Sprite Target 缺省为NULL，为NULL时从底端中间移动火焰图标到顶端中间
void BoardScene::spellmove(Sprite* Target) {
    // 创建法术图标精灵
    auto spell = Sprite::create("ImageElements/spell.png");
    Vec2 targetPos;
    if (isPlayer1Turn)
    {
        // 设置初始位置（底端中间）
        spell->setPosition(Vec2(2028 / 2, 0));
        this->addChild(spell);
        if (Target == NULL) {
            // 如果没有目标，移动到顶端中间
            targetPos = Vec2(2028 / 2, 1280);

        }
        else {
            // 如果有目标，移动到目标位置
            targetPos = Target->getPosition();
        }
    }
    else
    {
        // 设置初始位置（顶端中间）
        spell->setPosition(Vec2(2028 / 2, 1280));
        this->addChild(spell);
        if (Target == NULL) {
            // 如果没有目标，移动到底端中间
            targetPos = Vec2(2028 / 2, 0);

        }
        else {
            // 如果有目标，移动到目标位置
            targetPos = Target->getPosition();
        }
    }
    // 创建移动动作
    auto moveTo = MoveTo::create(0.8f, targetPos);

    // 创建淡出动作
    auto fadeOut = FadeOut::create(0.2f);

    // 创建动作序列（移动后淡出并移除）
    auto sequence = Sequence::create(
        moveTo,
        fadeOut,
        RemoveSelf::create(),
        nullptr
    );
    // 播放音效
    audioPlayer("Music/spell.mp3", false);
    // 执行动作
    spell->runAction(sequence);
}
// 处理随从对随从的攻击
void BoardScene::handleAttack(Sprite* attacker, Sprite* defender) {
    if (isPlayer1Turn)
    {
        hasAttackedThisTurn = true;  // 标记已经有随从攻击
        // 找到攻击者和防御者的索引
        int attackerIndex = -1;
        int defenderIndex = -1;
        // 查找攻击者索引
        for (size_t i = 0; i < playedCards.size(); i++) {
            if (playedCards[i] == attacker) {
                attackerIndex = i;
                break;
            }
        }
        // 查找防御者索引
        for (size_t i = 0; i < enemyPlayedCards.size(); i++) {
            if (enemyPlayedCards[i] == defender) {
                defenderIndex = i;
                break;
            }
        }
        // 播放攻击动画
        if (attackerIndex != -1 && defenderIndex != -1) {
            attackmove(attackerIndex, defenderIndex);
           
        }
        // 先复制需要的数据，避免后续访问可能被删除的数据
        int attackPower = 0;
        // 获取攻击力
        auto attackerIt = cardStats.find(attacker);
        if (attackerIt != cardStats.end()) {
            attackPower = attackerIt->second.attack;
        }
        else {
            return;
        }
        // 获取并更新防御者的生命值
        auto defenderIt = cardStats.find(defender);
        if (defenderIt != cardStats.end()) {
            defenderIt->second.health -= attackPower;

            // 先更新显示
            updateCardStats(defender);

            // 如果生命值小于等于0，移除卡牌
            if (defenderIt->second.health <= 0) {
                // 先从 cardStats 中移除
                cardStats.erase(defender);
                // 然后执行移除动画
                removeCardWithAnimation(defender);
            }
        }
        // 播放攻击音效
        audioPlayer("Music/attack.mp3", false);
    }
    else
    {
        // 找到攻击者和防御者的索引
        int attackerIndex = -1;
        int defenderIndex = -1;
        // 查找攻击者索引
        for (size_t i = 0; i < enemyPlayedCards.size(); i++) {
            if (enemyPlayedCards[i] == attacker) {
                attackerIndex = i;
                break;
            }
        }
        // 查找防御者索引
        for (size_t i = 0; i < playedCards.size(); i++) {
            if (playedCards[i] == defender) {
                defenderIndex = i;
                break;
            }
        }
        // 播放攻击动画
        if (attackerIndex != -1 && defenderIndex != -1) {
            attackmove(attackerIndex, defenderIndex);
            //Sleep(2000);
        }
        // 先复制需要的数据，避免后续访问可能被删除的数据
        int attackPower = 0;
        // 获取攻击力
        auto attackerIt = cardStats.find(attacker);
        if (attackerIt != cardStats.end()) {
            attackPower = attackerIt->second.attack;
        }
        else {
            return;
        }
        // 获取并更新防御者的生命值
        auto defenderIt = cardStats.find(defender);
        if (defenderIt != cardStats.end()) {
            defenderIt->second.health -= attackPower;

            // 先更新显示
            updateCardStats(defender);

            // 如果生命值小于等于0，移除卡牌
            if (defenderIt->second.health <= 0) {
                // 先从 cardStats 中移除
                cardStats.erase(defender);
                // 然后执行移除动画
                removeCardWithAnimation(defender);
            }
        }
        // 播放攻击音效
        audioPlayer("Music/attack.mp3", false);
    }
    // 更新UI
    updatePlayerUI();
    checkGameOver();
}
 
// 处理对英雄的攻击
void BoardScene::handleAttackToHero() {
    if (!attackingCard || !isPlayer1Turn) return;
    hasAttackedThisTurn = true;  // 标记已经有随从攻击
    // 检查卡牌是否存在于 cardStats 中
    if (cardStats.find(attackingCard) == cardStats.end()) {
        return;
    }
    // 获取攻击随从的攻击力
    auto& attackerStats = cardStats[attackingCard];
    // 扣除对方英雄生命值
    player2->health -= attackerStats.attack;
    // 更新UI
    updatePlayerUI();
    checkGameOver();
    // 播放攻击音效
    audioPlayer("Music/attack.mp3", false);
    //以下实现一个画面震动效果
    // 获取背景精灵（通过tag或名字）
    auto background = this->getChildByTag(0);  // 之前添加背景时设置的tag为0
    // 或者通过名字获取
    // auto background = this->getChildByName("background");

    if (background) {
        // 保存原始位置
        Vec2 originalPos = background->getPosition();

        // 创建震动序列
        auto shake = Sequence::create(
            MoveBy::create(0.02f, Vec2(-10, -5)),
            MoveBy::create(0.02f, Vec2(20, 10)),
            MoveBy::create(0.02f, Vec2(-20, -10)),
            MoveBy::create(0.02f, Vec2(20, 10)),
            MoveBy::create(0.02f, Vec2(-10, -5)),
            // 确保回到原始位置
            MoveTo::create(0, originalPos),
            nullptr
        );
       
        background->runAction(shake);
    }
}
 
// 移除卡牌并播放动画
void BoardScene::removeCardWithAnimation(Sprite* card) {
    if (!card) return;
    // 先从 cardStats 中移除
    cardStats.erase(card);

    // 卡牌消失动画
    card->runAction(Sequence::create(
        Spawn::create(
            FadeOut::create(0.5f),
            ScaleTo::create(0.5f, 0.1f),
            nullptr
        ),
        CallFunc::create([this, card]() {
            // 从数组中移除
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

    // 播放移除音效
    audioPlayer("Music/broken.mp3", false);
}
  
// 更新敌人的随从位置
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

// 点击开始处理
bool BoardScene::onTouchBegan(Touch* touch, Event* event) {

    if (isPlayer1Turn)
    {
        Vec2 touchLocation = touch->getLocation();

        // 如果已经选中了攻击随从，检查是否点击了有效目标
        if (attackingCard) {
            // 检查是否点击了敌方随从或敌方英雄区域
            bool targetFound = false;

            // 检查敌方随从
            for (auto enemyCard : enemyPlayedCards) {
                if (enemyCard->getBoundingBox().containsPoint(touchLocation)) {
                    handleAttack(attackingCard, enemyCard);
                    targetFound = true;
                    break;
                }
            }

            // 检查敌方英雄区域
            if (!targetFound && touchLocation.y > Director::getInstance()->getVisibleSize().height * 0.7f) {
                handleAttackToHero();
            }

            // 清除攻击状态
            attackingCard = nullptr;
            if (attackIndicator) {
                attackIndicator->removeFromParent();
                attackIndicator = nullptr;
            }
            return true;
        }

        // 检查是否点击了己方随从
        for (auto card : playedCards) {
            if (card->getBoundingBox().containsPoint(touchLocation)) {
                // 检查是否已经攻击过
                if (hasAttackedThisTurn) {
                    // 可以添加提示效果
                    return false;
                }
                attackingCard = card;
                createAttackIndicator(card->getPosition());
                return true;
            }
        }

        // 检查手牌区域的卡牌
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
    
// 点击拖动处理
void BoardScene::onTouchMoved(Touch* touch, Event* event) {
    if (selectedCard) {
        selectedCard->setPosition(selectedCard->getPosition() + touch->getDelta());
        checkDropArea();
    }
    else if (attackingCard && attackIndicator) {
        // 更新攻击指示器位置
        attackIndicator->clear();
        Vec2 startPos = attackingCard->getPosition();
        Vec2 currentPos = touch->getLocation();

        // 绘制攻击线
        attackIndicator->drawLine(startPos, currentPos, Color4F(1, 0, 0, 0.8f));
        // 绘制瞄准圈
        attackIndicator->drawCircle(currentPos, 30, 0, 360, false, Color4F(1, 0, 0, 0.8f));
    }
}
       
// 点击结束处理
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
            
// 将卡牌放回手牌
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
          
// 鼠标移动处理
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

// 缩放Sprite
void BoardScene::scaleSprite(Sprite* sprite, float scale) {
    sprite->runAction(ScaleTo::create(0.1f, scale));
}
    
// 出牌
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

        //如果该张牌是随从
        if (cardStats[sprite].type == "partner")
        {
            // 添加到场上
            playedCards.push_back(sprite);
            updatePlayedCardsPosition();
        }
        
        //如果该张牌是法术
        if (cardStats[sprite].type == "spell")
        {
            spellmove(NULL);
            // 扣除对方英雄生命值
            player2->health -= cardStats[sprite].attack;
            // 更新UI
            updatePlayerUI();
            checkGameOver();
            sprite->removeFromParent();
        }
        
        // 播放音效
        audioPlayer("music/putcard.mp3", false);
        // 更新剩余手牌位置
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
        
// 更新己方随从的位置
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
         
// 初始化玩家信息
void BoardScene::initPlayers() {
    player1 = new Player("Player1");
    player2 = new Player("Player2");
    isPlayer1Turn = true;
    player1->setPlayerCards();
    player2->setPlayerCards();
}
     
// 创建玩家UI
void BoardScene::createPlayerUI() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    // 创建己方玩家信息显示
    player1Health = Label::createWithTTF("HP: 30", "fonts/arial.ttf", 24);
    player1Health->setPosition(Vec2(800, 220));
    player1Health->enableOutline(Color4B::BLACK, 2);
    this->addChild(player1Health);
    player1Mana = Label::createWithTTF("Mana: 1/1", "fonts/arial.ttf", 24);
    player1Mana->setPosition(Vec2(800, 280));
    player1Mana->enableOutline(Color4B::BLACK, 2);
    this->addChild(player1Mana);
    // 创建对方玩家信息显示
    player2Health = Label::createWithTTF("HP: 30", "fonts/arial.ttf", 24);
    player2Health->setPosition(Vec2(800, visibleSize.height - 220));
    player2Health->enableOutline(Color4B::BLACK, 2);
    this->addChild(player2Health);
    player2Mana = Label::createWithTTF("Mana: 1/1", "fonts/arial.ttf", 24);
    player2Mana->setPosition(Vec2(800, visibleSize.height - 280));
    player2Mana->enableOutline(Color4B::BLACK, 2);
    this->addChild(player2Mana);
    // 回合指示器
    turnIndicator = Label::createWithTTF("Your Turn", "fonts/arial.ttf", 32);
    turnIndicator->setPosition(Vec2(1880, 600));
    turnIndicator->enableOutline(Color4B::BLACK, 2);
    this->addChild(turnIndicator);
    // 回合结束按钮
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

// 更新玩家UI
void BoardScene::updatePlayerUI() {
    // 更新生命值显示
    player1Health->setString("HP: " + std::to_string(player1->health));
    player2Health->setString("HP: " + std::to_string(player2->health));
    // 更新法力值显示
    player1Mana->setString("Mana: " + std::to_string(player1->money) + "/" +
        std::to_string(player1->maxmoney));
    player2Mana->setString("Mana: " + std::to_string(player2->money) + "/" +
        std::to_string(player2->maxmoney));
    // 更新回合指示
    turnIndicator->setString(isPlayer1Turn ? "Your Turn" : "Opponent's Turn");

}
    
// 回合切换
void BoardScene::switchTurn() {

    isPlayer1Turn = !isPlayer1Turn;
    hasAttackedThisTurn = false;  // 重置攻击标记
    // 加载点击音效
    audioPlayer("Music/ClickSoundEffect.mp3", false);

    // 切换回合时的逻辑
    Player* currentPlayer = isPlayer1Turn ? player1 : player2;
    currentPlayer->maxmoney = std::min(currentPlayer->maxmoney + 1, 10);
    currentPlayer->money = currentPlayer->maxmoney;
    // 更新UI
    updatePlayerUI();
    checkGameOver();
    // 回合切换动画
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
    // 切换回合时的逻辑
    currentPlayer = isPlayer1Turn ? player1 : player2;
    currentPlayer->maxmoney = std::min(currentPlayer->maxmoney + 1, 10);
    currentPlayer->money = currentPlayer->maxmoney;
    // 更新UI
    updatePlayerUI();
    checkGameOver();
    // 回合切换动画
    turnIndicator->runAction(Sequence::create(
        FadeOut::create(0.2f),
        CallFunc::create([this]() {
            updatePlayerUI();
            }),
        FadeIn::create(0.2f),
        nullptr
    ));
    // 回合开始时抽一张牌
    drawCard();
}
       

// 初始化牌堆
void BoardScene::initDecks() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    // 创建牌堆容器节点
    deckNode1 = Node::create();
    deckNode2 = Node::create();
    this->addChild(deckNode1);
    this->addChild(deckNode2);
    // 设置牌堆位置
    deckNode1->setPosition(Vec2(1950, 500));
    deckNode2->setPosition(Vec2(1950, 900));
    // 显示两个牌堆
    for (int i = 0; i < 30; i++) {
        // 创建玩家1的牌堆
        player1->playerCards.at(i)->setPosition(Vec2(-i * 2.0f, -i * 2.0f));
        deckNode1->addChild(player1->playerCards.at(i));

        // 创建玩家2的牌堆
        player2->playerCards.at(i)->setPosition(Vec2(-i * 2.0f, -i * 2.0f));
        deckNode2->addChild(player2->playerCards.at(i));
    }
}
         
// 从牌堆中抽一张牌
void BoardScene::drawCard() {
    if (isPlayer1Turn && !player1->playerCards.empty()) {
        // 从牌堆抽一张牌
        auto card = player1->playerCards.back();
        player1->playerCards.pop_back();
        // 创建新的可操作卡牌
        auto texture = card->getTexture();
        auto newCard = Sprite::createWithTexture(texture);
        newCard->setPosition(Vec2(1950, 500)); // 牌堆位置
        newCard->setScale(0.5f);
        // 计算目标位置
        Vec2 originalPos(CARD_REGION_X + dragCards.size() * (newCard->getContentSize().width + 30),
            CARD_REGION_Y);
        // 添加到待出牌区域并记录位置
        this->addChild(newCard);
        cardOriginalPositions[newCard] = originalPos;
        dragCards.push_back(newCard);

        // 随机生成卡牌属性
        int health = 1 + rand() % 7;
        int damage = 1 + rand() % 7;
        int cost = 1 + rand() % 7;
        string type = rand() % 2 ? "spell" : "partner";
        // 添加卡牌属性
        addCardStats(newCard, health, damage, cost, type);

        // 抽牌动画序列
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
        // 播放抽牌音效
        audioPlayer("Music/drawcard.mp3", false);
        // 更新牌堆显示
        deckNode1->removeChild(card);
    }
}

void BoardScene::AIplay() 
{
    // 从牌堆抽一张牌
    auto card = player2->playerCards.back();
    player2->playerCards.pop_back();
    auto texture = card->getTexture();
    card->removeFromParent();

    auto newCard = Sprite::createWithTexture(texture);
    int health = 1 + rand() % 5;  // 1-5的生命值
    int damage = 1 + rand() % 4;  // 1-4的攻击力
    int cost = 1 + rand() % 3;    // 1-3的费用
    string type = (rand() % 100 < 50) ? "partner" : "spell";  // 50%概率生成随从

    addCardStats(newCard, health, damage, cost, type);
    if (type == "partner") {

        // 播放出牌音效
        audioPlayer("Music/putcard.mp3", false);
        newCard->setScale(0.8f);
        // 添加到场上
        enemyPlayedCards.push_back(newCard);
        this->addChild(newCard);
        updateEnemyCardsPosition();
        
    }
    else {
        // 播放出牌音效
        audioPlayer("Music/putcard.mp3", false);
        newCard->setScale(0.8f);
        // 添加到场上
        enemyPlayedCards.push_back(newCard);
        this->addChild(newCard);
        updateEnemyCardsPosition();

        enemyPlayedCards.pop_back();
        // 缩放一段时间让玩家看清
        // 设置目标放大和缩小的比例
        float scaleUpFactor = 2.0f;   // 放大倍数
        float scaleDownFactor = 0.0f; // 缩小到消失，最终大小为 0
        // 创建放大动作，3秒钟内从当前大小放大到 2 倍
        auto scaleUpAction = cocos2d::ScaleTo::create(1.5f, scaleUpFactor); // 放大 1.5 秒
        // 创建缩小动作，2秒钟内从放大后的状态缩小到 0
        auto scaleDownAction = cocos2d::ScaleTo::create(1.5f, scaleDownFactor); // 缩小 1.5 秒
        // 创建消失动作，3秒钟内透明度变为 0
        auto fadeOutAction = cocos2d::FadeOut::create(1.5f);  // 透明度变化持续 1.5 秒
        // 在动作结束后移除精灵
        newCard->runAction(cocos2d::Sequence::create(
            scaleUpAction,
            scaleDownAction,
            fadeOutAction,
            cocos2d::CallFunc::create([newCard]() {
                newCard->removeFromParent();  // 动作完成后移除精灵
                }), nullptr
        ));
        updateEnemyCardsPosition();
       
        // 使用法术卡直接造成伤害
        spellmove(nullptr);
        player1->health -= damage;
        audioPlayer("Music/spell.mp3", false);
    }

    // AI随机攻击
    if (!enemyPlayedCards.empty()) {
        auto attackerCard = enemyPlayedCards.at(rand() % enemyPlayedCards.size());//随机挑选一个ai方随从
        // 80%概率发动攻击
        if (rand() % 100 < 80) {
            if (!playedCards.empty()) {
                // 随机选择一个玩家随从攻击
                int targetIndex = rand() % playedCards.size();
                handleAttack(attackerCard, playedCards[targetIndex]);
            }
            else {
                // 直接攻击玩家英雄
                player1->health -= cardStats[attackerCard].attack;
                audioPlayer("Music/attack.mp3", false);
                //Sleep(1000);

            }
        }
        
    }
    updatePlayerUI();
    checkGameOver();
}

// 添加游戏结束检查
void BoardScene::checkGameOver() {
    if (isGameOver) return;

    if (player1->health <= 0 || player2->health <= 0) {
        isGameOver = true;

        // 创建游戏结束标签
        gameOverLabel = Label::createWithTTF(
            player1->health <= 0 ? "You Lose" : "You Win",
            "fonts/arial.ttf",
            120
        );

        auto visibleSize = Director::getInstance()->getVisibleSize();
        gameOverLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        gameOverLabel->enableOutline(Color4B::BLACK, 4);
        this->addChild(gameOverLabel, 100);

        // 添加动画效果
        gameOverLabel->setScale(0.1f);
        gameOverLabel->runAction(Sequence::create(
            EaseElasticOut::create(ScaleTo::create(1.0f, 1.0f)),
            nullptr
        ));
    }
}
// 取消按钮回调
void BoardScene::cancelCallback(Ref* pSender) {
    // 加载点击音效
    audioPlayer("Music/ClickSoundEffect.mp3", false);
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene()));
}

