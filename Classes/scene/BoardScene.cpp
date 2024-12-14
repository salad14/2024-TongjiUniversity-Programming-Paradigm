/****************************************************************
* 棋盘页面的实现
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
        // 设置图片的尺寸为窗口的尺寸
        title_sprite->setContentSize(visibleSize);
        // 将图片的位置调整为窗口的中心
        title_sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(title_sprite, 0);
    }

    //// 创建添加卡牌的按钮(暂时用于测试)
    //auto addButton = MenuItemImage::create("button/addcard.png", "button/addcard.png", CC_CALLBACK_0(BoardScene::addNewCard, this));
    //addButton->setPosition(Vec2(1800, 400));
   
    // 创建返回按钮
    auto cancel = MenuItemImage::create("button/cancel.png", "button/cancelSelected.png", CC_CALLBACK_1(BoardScene::cancelCallback, this));
    cancel->setPosition(Vec2(1940,20));
    auto menu = Menu::create(cancel, nullptr);
    // 添加菜单
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
    
    // 创建移动监听器
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = CC_CALLBACK_1(BoardScene::onMouseMove, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    hoveredCard = nullptr;


    // 创建中央出牌区域
    createDropArea();

    // 启用update,按帧刷新
    //this->scheduleUpdate();
    initPlayers();
    createPlayerUI();
    updatePlayerUI();
    initDecks();  // 初始化牌堆

    return true;
}

//返回主菜单
void BoardScene::cancelCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("Music/ClickSoundEffect.mp3", false);
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene()));
}

// 创建中央出牌区域
void BoardScene::createDropArea() 
{
    dropArea = DrawNode::create();

    // 获取屏幕中心点
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 center = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

    // 绘制半透明的矩形区域
    Vec2 vertices[] = 
    {
        Vec2(center.x - PUTOUT_CARD_REGION_HALF_X, center.y - PUTOUT_CARD_REGION_HALF_Y),
        Vec2(center.x + PUTOUT_CARD_REGION_HALF_X, center.y - PUTOUT_CARD_REGION_HALF_Y),
        Vec2(center.x + PUTOUT_CARD_REGION_HALF_X, center.y + PUTOUT_CARD_REGION_HALF_Y),
        Vec2(center.x - PUTOUT_CARD_REGION_HALF_X, center.y + PUTOUT_CARD_REGION_HALF_Y)
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
        abs(spritePos.x - center.x) <= PUTOUT_CARD_REGION_HALF_X &&
        abs(spritePos.y - center.y) <= PUTOUT_CARD_REGION_HALF_Y;

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


//从出牌区域移除一张卡牌
void BoardScene::removeCard(Sprite* sprite) {
    if (!sprite) return;

    if (sprite == hoveredCard) {
        hoveredCard = nullptr;
    }

    // 从手牌移除
    auto iter = find(dragCards.begin(), dragCards.end(), sprite);
    if (iter != dragCards.end()) {
        // 获取被移除卡牌的索引位置
        size_t removedIndex = distance(dragCards.begin(), iter);

        dragCards.erase(iter);
        cardOriginalPositions.erase(sprite);
        playedCards.push_back(sprite);
        updatePlayedCardsPosition();

        // 播放音效
        audioPlayer("music/putcard.mp3", false);

        // 更新剩余卡牌的位置
        for (size_t i = removedIndex; i < dragCards.size(); i++) {
            Sprite* card = dragCards[i];
            // 计算新位置
            Vec2 newPos(CARD_REGION_X + i * (card->getContentSize().width + 30), CARD_REGION_Y);

            // 更新存储的原始位置
            cardOriginalPositions[card] = newPos;

            // 添加移动动画
            card->runAction(Sequence::create(
                EaseInOut::create(MoveTo::create(0.3f, newPos), 2.0f),
                EaseElasticOut::create(ScaleTo::create(0.2f, 1.0f)),
                nullptr
            ));
        }

        
    }
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
            abs(finalPos.x - center.x) <= PUTOUT_CARD_REGION_HALF_X &&
            abs(finalPos.y - center.y) <= PUTOUT_CARD_REGION_HALF_Y;

        Sprite* cardToHandle = selectedCard;
        selectedCard = nullptr;  // 先清除选中状态

        if (inDropArea) {
            // 在出牌区域内，执行移除
           // removeCard(cardToHandle);
            Player* currentPlayer = isPlayer1Turn ? player1 : player2;

            // 这里需要添加获取卡牌费用的逻辑
            int cardCost = 1; // 示例费用

            if (currentPlayer->money >= cardCost) {
                currentPlayer->money -= cardCost;
                removeCard(cardToHandle);
                updatePlayerUI();
            }
            else 
            {
                // 费用不足，回到原位
                Vec2 originalPos = cardOriginalPositions[cardToHandle];
                cardToHandle->runAction(EaseBackOut::create(MoveTo::create(0.5f, originalPos)));
            }
        }
        else 
        {
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

// 初始化玩家
void BoardScene::initPlayers() 
{
    player1 = new Player("Player1");
    player2 = new Player("Player2");
    isPlayer1Turn = true;  // 默认玩家1先手

    // 初始化玩家卡组
    
    // ... 初始化卡组(待完善)
    //详见 player.h及player.cpp
    player1->setPlayerCards();
    player2->setPlayerCards();
}

// 创建玩家信息UI
void BoardScene::createPlayerUI() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建己方玩家信息显示
    player1Health = Label::createWithTTF("HP: 30", "fonts/arial.ttf", 24);
    player1Health->setPosition(Vec2(800, 220));
    this->addChild(player1Health);
    // 设置描边（同时设置颜色和粗细）
    player1Health->enableOutline(Color4B::BLACK, 2);         // 黑色描边，粗细为2

    player1Mana = Label::createWithTTF("Mana: 1/1", "fonts/arial.ttf", 24);
    player1Mana->setPosition(Vec2(800, 280));
    this->addChild(player1Mana);
    // 设置描边（同时设置颜色和粗细）
    player1Mana->enableOutline(Color4B::BLACK, 2);         // 黑色描边，粗细为2

    // 创建对方玩家信息显示
    player2Health = Label::createWithTTF("HP: 30", "fonts/arial.ttf", 24);
    player2Health->setPosition(Vec2(800, visibleSize.height - 220));
    this->addChild(player2Health);
    player2Health->enableOutline(Color4B::BLACK, 2);         // 黑色描边，粗细为2

    player2Mana = Label::createWithTTF("Mana: 1/1", "fonts/arial.ttf", 24);
    player2Mana->setPosition(Vec2(800, visibleSize.height - 280));
    this->addChild(player2Mana);
    player2Mana->enableOutline(Color4B::BLACK, 2);         // 黑色描边，粗细为2

    // 回合指示器
    turnIndicator = Label::createWithTTF("Your Turn", "fonts/arial.ttf", 32);
    turnIndicator->setPosition(Vec2(1880, 600));
    this->addChild(turnIndicator);
    // 设置描边（同时设置颜色和粗细）
    turnIndicator->enableOutline(Color4B::BLACK, 2);         // 黑色描边，粗细为2

    // 添加回合结束按钮
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

// 切换回合
void BoardScene::switchTurn() 
{
    isPlayer1Turn = !isPlayer1Turn;
    // 加载点击音效
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
    // 切换回合时的逻辑
    Player* currentPlayer = isPlayer1Turn ? player1 : player2;
    currentPlayer->maxmoney = std::min(currentPlayer->maxmoney + 1, 10);
    currentPlayer->money = currentPlayer->maxmoney;

    // 更新UI
    updatePlayerUI();

    // 可以添加回合切换动画
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
    for (int i = 0; i < 30; i++) 
    {
        // 创建玩家1的牌堆
        player1->playerCards.at(i)->setPosition(Vec2(-i * 2.0f, -i * 2.0f));
        deckNode1->addChild(player1->playerCards.at(i));
        // 创建玩家2的牌堆
        player2->playerCards.at(i)->setPosition(Vec2(-i * 2.0f, -i * 2.0f));
        deckNode2->addChild(player2->playerCards.at(i));
    }
}

// 从卡组牌堆中抽一张牌
void BoardScene::drawCard() {
    if (isPlayer1Turn && !player1->playerCards.empty()) {
        // 从牌堆抽一张牌
        auto card = player1->playerCards.back();
        player1->playerCards.pop_back();

        // 创建新的可操作卡牌，复制card的纹理
        auto texture = card->getTexture();
        auto newCard = Sprite::createWithTexture(texture);

        // 设置新卡牌的初始位置（从牌堆位置开始）
        newCard->setPosition(Vec2(1950, 500)); // 牌堆位置
        newCard->setScale(0.5f); // 初始较小的尺寸

        // 计算目标位置
        Vec2 originalPos(CARD_REGION_X + dragCards.size() * (newCard->getContentSize().width + 30), CARD_REGION_Y);

        // 添加到待出牌区域并记录位置
        this->addChild(newCard);
        cardOriginalPositions[newCard] = originalPos;
        dragCards.push_back(newCard);

        // 抽牌动画序列
        newCard->runAction(Sequence::create(
            // 1. 先稍微上浮
            EaseOut::create(MoveBy::create(0.2f, Vec2(0, 50)), 2.0f),
            // 2. 移动到目标位置并放大
            Spawn::create(
                EaseInOut::create(MoveTo::create(0.5f, originalPos), 2.0f),
                EaseInOut::create(ScaleTo::create(0.5f, 1.0f), 2.0f),
                RotateBy::create(0.5f, 360), // 旋转一圈
                nullptr
            ),
            // 3. 最后轻微弹跳效果
            EaseElasticOut::create(ScaleTo::create(0.3f, 1.0f)),
            nullptr
        ));

        // 播放抽牌音效
        audioPlayer("Music/drawcard.mp3", false);

        // 更新牌堆显示
        deckNode1->removeChild(card);
    }
    else if (!player2->playerCards.empty()) {
        // 第二名玩家的抽牌逻辑（待完成）
    }
}

// 更新场上已打出卡牌的显示
void BoardScene::updatePlayedCardsPosition() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    float cardWidth = 100;  // 假设卡牌宽度为100
    float spacing = 20;     // 卡牌间距

    // 计算起始位置（使卡牌居中显示）
    float startX = center.x - (playedCards.size() * (cardWidth + spacing) - spacing) / 2;

    // 更新每张已打出卡牌的位置
    for (size_t i = 0; i < playedCards.size(); i++) {
        Sprite* card = playedCards[i];
        Vec2 targetPos = Vec2(startX + i * (cardWidth + spacing), center.y);

        // 使用动画移动到目标位置
        card->runAction(EaseBackOut::create(MoveTo::create(0.3f, targetPos)));
    }
}