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

// 使用 cocos2d 命名空间
USING_NS_CC;

// 使用命名空间别名简化 Photon SDK 的类型引用
namespace EG = ExitGames::Common;

// 定义 cardNumber 为 int
typedef int CardNumber;

// 定义玩家编号类型
typedef int PlayerNumber;

// 创建场景
Scene* BoardScene::createScene() {
    return BoardScene::create();
}

// 打印加载错误信息
static void problemLoading(const char* filename) {
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in BoardScene.cpp\n");
}

// 初始化
bool BoardScene::init() {
    if (!Scene::init()) {
        return false;
    }

    // 获取全局 PhotonLib 实例
    photonLib = PhotonLib::getInstance();
    if (!photonLib) {
        CCLOG("Failed to get PhotonLib instance.");
        CocosUIListener::getInstance()->writeString(EG::JString(L"Failed to get PhotonLib instance."));
        return false;
    }

    // 获取全局 CocosUIListener 实例
    cocosUIListener = CocosUIListener::getInstance();
    if (!cocosUIListener) {
        CCLOG("Failed to get CocosUIListener instance.");
        return false;
    }

    // 创建一个用于UI的Layer（如果MainScene中已经有，可以省略）
    auto uiLayer = Layer::create();
    this->addChild(uiLayer, 100); // 确保UI层在最上层

    // 将 CocosUIListener 附加到 UI Layer 上
    cocosUIListener->attachToLayer(uiLayer, Vec2(100, 100)); // 根据需要调整位置

    // 设置 PhotonLib 的自定义事件回调
    photonLib->setCustomEventCallback(CC_CALLBACK_2(BoardScene::onPhotonEvent, this));

    // 初始化玩家
    initPlayers();

    // 创建玩家信息UI
    createPlayerUI();

    // 获取屏幕尺寸和原点
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 添加背景图片
    auto title_sprite = Sprite::create("board.png");
    if (title_sprite == nullptr) {
        problemLoading("board.png");
        cocosUIListener->writeString(EG::JString(L"Failed to load board.png"));
        return false;
    }
    else {
        // 设置图片的尺寸为窗口的尺寸
        title_sprite->setContentSize(visibleSize);
        // 将图片的位置调整为窗口的中心
        title_sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(title_sprite, 0);
    }

    // 创建返回按钮
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
        // 使用第一次提供的参数
        cancel->setPosition(Vec2(1940, 20)); // 与第一次实现一致
    }
    auto menu = Menu::create(cancel, nullptr);

    // 添加菜单
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // 创建触摸监听器
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(BoardScene::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(BoardScene::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(BoardScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    selectedCard = nullptr;

    // 创建鼠标监听器
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = CC_CALLBACK_1(BoardScene::onMouseMove, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    hoveredCard = nullptr;

    // 创建中央出牌区域
    createDropArea();

    // 添加 update 方法的调用
    this->scheduleUpdate();

    // 通过事件分发初始手牌，每个玩家初始抽3张牌
    distributeInitialHands(); // 通过事件分发抽牌

    // 启动第一个玩家的回合
    sendTurnStartEvent(); // 触发第一个玩家的回合开始

    return true;
}

// 返回主菜单
void BoardScene::cancelCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("Music/ClickSoundEffect.mp3", false);
    // 离开房间
    photonLib->leaveRoom();
    cocosUIListener->writeString(EG::JString(L"Leaving room and returning to main menu."));
    // 切换到主菜单
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene(), Color3B::WHITE));
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
    this->addChild(dropArea, 1);
}

// 定义 checkDropArea 函数
void BoardScene::checkDropArea() {
    if (!selectedCard)
        return;

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2 + Director::getInstance()->getVisibleOrigin().x,
        visibleSize.height / 2 + Director::getInstance()->getVisibleOrigin().y);
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
                5.0f,                        // 边框宽度
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

// 鼠标移动检测
void BoardScene::onMouseMove(Event* event) {
    EventMouse* mouseEvent = dynamic_cast<EventMouse*>(event);
    Vec2 mousePos = Vec2(mouseEvent->getCursorX(), mouseEvent->getCursorY());

    // 检查鼠标是否悬停在某个精灵上
    Sprite* newHoveredSprite = nullptr;

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

// 平滑缩放精灵
void BoardScene::scaleSprite(Sprite* sprite, float scale) {
    // 使用动作实现平滑的缩放效果
    sprite->stopAllActions();
    sprite->runAction(ScaleTo::create(0.1f, scale));
}

// 触摸开始
bool BoardScene::onTouchBegan(Touch* touch, Event* event)
{
    // 只有在本地玩家的回合才允许出牌
    if (!isLocalPlayerTurn) {
        return false;
    }

    Vec2 touchLocation = touch->getLocation();

    // 从后向前检查（使最上层的精灵优先响应）
    for (auto it = localPlayerCards.rbegin(); it != localPlayerCards.rend(); ++it) {
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

// 触摸移动
void BoardScene::onTouchMoved(Touch* touch, Event* event)
{
    if (selectedCard)
    {
        selectedCard->setPosition(selectedCard->getPosition() + touch->getDelta());
        checkDropArea();
    }
}

// 触摸结束
// 触摸结束
void BoardScene::onTouchEnded(Touch* touch, Event* event)
{
    if (selectedCard)
    {
        // 先检查卡牌是否在出牌区域
        checkDropArea();

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
            // 使用 currentPlayerNumber 确定当前玩家
            players::Player* currentPlayer = (currentPlayerNumber == 1) ? player1 : player2;
            PlayerNumber playerNumber = currentPlayerNumber; // 当前回合玩家编号

            // 获取卡牌费用的逻辑（假设卡牌费用存储在卡牌的 Tag 或其他属性中）
            int cardCost = getCardCost(cardToHandle); // 需要实现 getCardCost 方法

            if (currentPlayer->getMoney() >= cardCost) {
                // 扣除法力值
                currentPlayer->setMoney(currentPlayer->getMoney() - cardCost);

                // 获取卡牌编号
                CardNumber cardNumber = cardToHandle->getTag();

                // 发送打牌事件，包含 playerNumber 和 cardNumber
                sendPlayCardEvent(playerNumber, cardNumber);

                // 移除卡牌并更新 UI
                removeCard(cardToHandle);
                updatePlayerUI();

                // 播放打牌音效
                audioPlayer("Music/playcard.mp3", false);
            }
            else
            {
                // 费用不足，回到原位
                Vec2 originalPos = cardOriginalPositions[cardToHandle];
                cardToHandle->runAction(EaseBackOut::create(MoveTo::create(0.5f, originalPos)));

                // 恢复颜色和移除描边
                cardToHandle->setColor(Color3B::WHITE);
                auto outline = cardToHandle->getChildByName("outline");
                if (outline) {
                    outline->removeFromParent();
                }

                // 显示费用不足的信息
                cocosUIListener->writeString(EG::JString(L"Not enough mana to play the card."));
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

// 获取卡牌费用的方法（示例实现）
int BoardScene::getCardCost(Sprite* card) {
    // 假设卡牌费用存储在 Tag 中，或通过其他方式获取
    // 这里使用 Tag 作为示例
    return card->getTag(); // 需要根据实际情况调整
}

// 初始化玩家
// BoardScene.cpp

void BoardScene::initPlayers()
{
    localPlayerNumber = photonLib->getLocalPlayerNumber();
    currentPlayerNumber = 1;
    player1 = GameData::getInstance().getPlayer1();
    player2 = GameData::getInstance().getPlayer2();

    // 根据 currentPlayerNumber 和 localPlayerNumber 设置 isLocalPlayerTurn
    isLocalPlayerTurn = (currentPlayerNumber == localPlayerNumber);
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
    turnIndicator->setPosition(Vec2(1880, 600)); // 与第一次实现一致
    this->addChild(turnIndicator);
    // 设置描边（同时设置颜色和粗细）
    turnIndicator->enableOutline(Color4B::BLACK, 2);         // 黑色描边，粗细为2

    // 添加回合结束按钮（仅本地玩家可以看到）
    auto endTurnBtn = MenuItemImage::create(
        "button/endturn.png",
        "button/endturnSelected.png",
        [this](Ref* sender) {
            this->switchTurn();
        }
    );
    endTurnBtn->setPosition(Vec2(1880, 700)); // 与第一次实现一致
    auto menu = Menu::create(endTurnBtn, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
}

// 更新玩家UI
void BoardScene::updatePlayerUI() {
    // 更新生命值显示
    player1Health->setString("HP: " + std::to_string(player1->getHealth()));
    player2Health->setString("HP: " + std::to_string(player2->getHealth()));

    // 更新法力值显示
    player1Mana->setString("Mana: " + std::to_string(player1->getMoney()) + "/" +
        std::to_string(player1->getMaxMoney()));
    player2Mana->setString("Mana: " + std::to_string(player2->getMoney()) + "/" +
        std::to_string(player2->getMaxMoney()));

    // 更新回合指示
    if (isLocalPlayerTurn) {
        turnIndicator->setString("Your Turn");
    }
    else {
        turnIndicator->setString("Opponent's Turn");
    }

    // 获取屏幕尺寸
    Size visibleSize = Director::getInstance()->getVisibleSize();

    // 获取本地玩家
    players::Player* localPlayer = (localPlayerNumber == 1) ? player1 : player2;
    int yPos = (localPlayerNumber == 1) ? CARD_REGION_Y : (visibleSize.height - CARD_REGION_Y);

    // 清除当前所有本地玩家的卡牌精灵位置
    for (auto& card : localPlayerCards) {
        // 每次更新前停止所有动作
        card->stopAllActions();
    }

    // 更新本地玩家的手牌位置
    for (size_t i = 0; i < localPlayer->getHand().size(); ++i) {
        CardNumber cardNumber = localPlayer->getHand()[i];
        cocos2d::Sprite* card = findCardByID(cardNumber); // 获取 Sprite by cardNumber
        CCLOG("Updating Local Player's card at index %zu: %p for cardNumber: %d", i, card, cardNumber);
        if (card) {
            // 统一本地玩家卡牌的位置
            Vec2 pos = Vec2(CARD_REGION_X + i * (card->getContentSize().width + 30), yPos);
            card->runAction(MoveTo::create(0.3f, pos));
            cardOriginalPositions[card] = pos;
        }
        else {
            CCLOG("Local Player's card with cardNumber %d is null or invalid.", cardNumber);
        }
    }

    // 对手玩家的手牌无需更新位置或显示
    // 如果需要，可以更新对手手牌数量的标签
}

// 切换回合
// 切换回合
void BoardScene::switchTurn()
{
    if (!isLocalPlayerTurn) {
        CCLOG("It's not your turn.");
        cocosUIListener->writeString(EG::JString(L"It's not your turn."));
        return;
    }

    // 计算下一个玩家编号（在2人游戏中，玩家编号为1或2）
    int nextPlayerNumber = (currentPlayerNumber == 1) ? 2 : 1;

    // 更新当前玩家编号
    currentPlayerNumber = nextPlayerNumber;

    // 更新是否为本地玩家的回合
    isLocalPlayerTurn = (currentPlayerNumber == localPlayerNumber);

    // 发送 TURN_START 事件，通知所有玩家回合开始
    sendTurnStartEvent();

    // 更新 UI
    updatePlayerUI();

    CCLOG("Switched turn to playerNumber: %d", currentPlayerNumber);
    cocosUIListener->writeString(EG::JString(L"Switched turn."));
}

// 发送回合开始事件
void BoardScene::sendTurnStartEvent() {
    EG::Hashtable eventContent;
    // 传递当前玩家编号
    PlayerNumber currentPlayerNum = currentPlayerNumber;
    eventContent.put(static_cast<unsigned char>(0), EG::Helpers::ValueToObject<EG::Object>::get(currentPlayerNum));

    photonLib->raiseCustomEvent(eventContent, TURN_START, ExitGames::Lite::ReceiverGroup::ALL);
    CCLOG("Sent TURN_START event for playerNumber: %d", currentPlayerNum);
    cocosUIListener->writeString(EG::JString(L"Sent TURN_START event."));
}

// 从卡牌移除并添加到战场
void BoardScene::removeCard(Sprite* sprite) {
    if (!sprite) return;

    if (sprite == hoveredCard) {
        hoveredCard = nullptr;
    }

    // 从 localPlayerCards 移除
    auto iter = std::find(localPlayerCards.begin(), localPlayerCards.end(), sprite);
    if (iter != localPlayerCards.end()) {
        localPlayerCards.erase(iter);
    }

    // 从 cardOriginalPositions 移除
    cardOriginalPositions.erase(sprite);

    // 添加到已打出卡牌区域
    playedCards.push_back(sprite);
    updatePlayedCardsPosition();

    // 从玩家的手牌中移除卡牌编号
    CardNumber cardNumber = sprite->getTag();
    players::Player* currentPlayer = (currentPlayerNumber == 1) ? player1 : player2;
    currentPlayer->removeCardFromHand(cardNumber);

    // 从场景中移除 sprite
    sprite->removeFromParent();

    // 播放音效
    audioPlayer("Music/putcard.mp3", false);
}


// 更新场上已打出卡牌的显示
void BoardScene::updatePlayedCardsPosition() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    float cardWidth = 100;  // 假设卡牌宽度为100
    float spacing = 20;     // 卡牌间距

    // 计算起始位置（使卡牌居中显示）
    float totalWidth = playedCards.size() * (cardWidth + spacing) - spacing;
    float startX = center.x - totalWidth / 2;

    // 更新每张已打出卡牌的位置
    for (size_t i = 0; i < playedCards.size(); i++) {
        Sprite* card = playedCards[i];
        Vec2 targetPos = Vec2(startX + i * (cardWidth + spacing), center.y);

        // 使用动画移动到目标位置
        card->runAction(EaseBackOut::create(MoveTo::create(0.3f, targetPos)));
    }
}

// 发送打牌事件
void BoardScene::sendPlayCardEvent(PlayerNumber playerNumber, CardNumber cardNumber) {
    EG::Hashtable eventContent;
    // 使用不同的 key 来区分 playerNumber 和 cardNumber
    eventContent.put(static_cast<unsigned char>(0), EG::Helpers::ValueToObject<EG::Object>::get(playerNumber));
    eventContent.put(static_cast<unsigned char>(1), EG::Helpers::ValueToObject<EG::Object>::get(cardNumber));

    photonLib->raiseCustomEvent(eventContent, PLAY_CARD, ExitGames::Lite::ReceiverGroup::ALL);
    CCLOG("Sent PLAY_CARD event with playerNumber: %d, cardNumber: %d", playerNumber, cardNumber);
    cocosUIListener->writeString(EG::JString(L"Sent PLAY_CARD event with playerNumber: ") +
        EG::JString(std::to_wstring(playerNumber).c_str()) +
        EG::JString(L", cardNumber: ") +
        EG::JString(std::to_wstring(cardNumber).c_str()));
}

// 发送抽牌事件
void BoardScene::sendDrawCardEvent(PlayerNumber playerNumber, CardNumber cardNumber) {
    EG::Hashtable eventContent;

    // 使用 ValueToObject 辅助类将 int 转换为 Object
    EG::Object playerObj = EG::Helpers::ValueToObject<EG::Object>::get(playerNumber);
    EG::Object cardObj = EG::Helpers::ValueToObject<EG::Object>::get(cardNumber);

    eventContent.put(static_cast<unsigned char>(0), playerObj);
    eventContent.put(static_cast<unsigned char>(1), cardObj);

    // 打印发送的对象类型
    CCLOG("Sending DRAW_CARD event:");
    CCLOG("playerObj Type: %d", playerObj.getType()); // 假设 getType() 返回 TypeCode
    CCLOG("cardObj Type: %d", cardObj.getType());

    photonLib->raiseCustomEvent(eventContent, DRAW_CARD, ExitGames::Lite::ReceiverGroup::ALL);
    CCLOG("Sent DRAW_CARD event with playerNumber: %d, cardNumber: %d", playerNumber, cardNumber);
    cocosUIListener->writeString(EG::JString(L"Sent DRAW_CARD event with playerNumber: ") +
        EG::JString(std::to_wstring(playerNumber).c_str()) +
        EG::JString(L", cardNumber: ") +
        EG::JString(std::to_wstring(cardNumber).c_str()));
}

// 处理 Photon 自定义事件
void BoardScene::onPhotonEvent(int eventCode, const EG::Hashtable& parameters) {
    switch (eventCode) {
        case PLAY_CARD:
            handlePlayCard(parameters);
            break;
        case DRAW_CARD:
            handleDrawCard(parameters);
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

// 处理打牌事件
void BoardScene::handlePlayCard(const EG::Hashtable& parameters) {
    int playerNumber = 0;
    int cardNumber = 0;

    // 获取 playerNumber
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

    // 获取 cardNumber
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

    // 确定目标玩家
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

    // 仅处理本地玩家的打牌事件
    if ((playerNumber == 1 && localPlayerNumber == 1) ||
        (playerNumber == 2 && localPlayerNumber == 2)) {
        // 找到对应的卡牌
        Sprite* card = findCardByID(cardNumber);

        if (card) {
            // 将卡牌从手牌移除并添加到战场
            removeCard(card);
            updatePlayerUI();
            cocosUIListener->writeString(EG::JString(L"Handled PLAY_CARD event for cardNumber: ") +
                EG::JString(std::to_wstring(cardNumber).c_str()) +
                EG::JString(L" by playerNumber: ") +
                EG::JString(std::to_wstring(playerNumber).c_str()));
        }
        else {
            CCLOG("Card with cardNumber: %d not found in local player's hand.", cardNumber);
            cocosUIListener->writeString(EG::JString(L"Card with cardNumber: ") +
                EG::JString(std::to_wstring(cardNumber).c_str()) +
                EG::JString(L" not found in local player's hand."));
        }
    }
    else {
        // 如果是对手玩家的打牌事件，更新对手玩家的状态但不创建或修改任何精灵
        CCLOG("Opponent player %d played cardNumber: %d. No sprite to handle.", playerNumber, cardNumber);
        cocosUIListener->writeString(EG::JString(L"Opponent played a card."));
        // 仅更新对手玩家的手牌数据
        targetPlayer->removeCardFromHand(cardNumber);
        // 可选：在UI上显示对手已打出一张卡牌的提示
    }
}

// 处理回合开始事件
void BoardScene::handleTurnStart(const EG::Hashtable& parameters) {
    int receivedPlayerNumber = 0;

    // 获取 receivedPlayerNumber
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

    // 更新当前玩家编号和本地回合状态
    currentPlayerNumber = receivedPlayerNumber;
    isLocalPlayerTurn = (currentPlayerNumber == localPlayerNumber);

    // 更新UI
    updatePlayerUI();

    // 如果是本地玩家的回合，抽一张卡牌
    if (isLocalPlayerTurn) {
        players::Player* currentPlayer = (currentPlayerNumber == 1) ? player1 : player2;
        if (currentPlayer->hasCards()) {
            CardNumber cardNumber = currentPlayer->drawCard(); // 抽取一张卡牌
            if (cardNumber != -1) {
                // 发送 DRAW_CARD 事件，包含卡牌ID和玩家编号
                sendDrawCardEvent(currentPlayer->getPlayerNumber(), cardNumber);
                CCLOG("Player %d drew cardNumber: %d", currentPlayer->getPlayerNumber(), cardNumber);
                cocosUIListener->writeString(EG::JString(L"Player drew a card."));
            }
        }
        else {
            CCLOG("Player %d has no cards to draw.", currentPlayer->getPlayerNumber());
            cocosUIListener->writeString(EG::JString(L"Player has no cards to draw."));
            // 处理牌库为空的逻辑，例如判负
            endGame((currentPlayerNumber == 1) ? player2 : player1);
        }
    }

    cocosUIListener->writeString(EG::JString(L"Handled TURN_START event. Switched turn."));
}

// 处理抽牌事件
void BoardScene::handleDrawCard(const EG::Hashtable& parameters) {
    int playerNumber = 0;
    int cardNumber = 0;

    // 获取 playerNumber
    const EG::Object* objPlayerNumber = parameters.getValue(static_cast<unsigned char>(0));
    if (objPlayerNumber && objPlayerNumber->getType() == EG::TypeCode::INTEGER) {
        const EG::ValueObject<int>* voPlayerNumber = static_cast<const EG::ValueObject<int>*>(objPlayerNumber);
        if (voPlayerNumber) {
            playerNumber = voPlayerNumber->getDataCopy();
            CCLOG("Received DRAW_CARD for playerNumber: %d", playerNumber);
        }
        else {
            CCLOG("Failed to cast playerNumber in DRAW_CARD event. Actual TypeCode: %d", objPlayerNumber->getType());
            cocosUIListener->writeString(EG::JString(L"Failed to cast playerNumber in DRAW_CARD event."));
            return;
        }
    }
    else {
        CCLOG("playerNumber not found or incorrect TypeCode in DRAW_CARD event.");
        cocosUIListener->writeString(EG::JString(L"playerNumber not found or incorrect TypeCode in DRAW_CARD event."));
        return;
    }

    // 获取 cardNumber
    const EG::Object* objCardNumber = parameters.getValue(static_cast<unsigned char>(1));
    if (objCardNumber && objCardNumber->getType() == EG::TypeCode::INTEGER) {
        const EG::ValueObject<int>* voCardNumber = static_cast<const EG::ValueObject<int>*>(objCardNumber);
        if (voCardNumber) {
            cardNumber = voCardNumber->getDataCopy();
            CCLOG("Received DRAW_CARD with cardNumber: %d for playerNumber: %d", cardNumber, playerNumber);
        }
        else {
            CCLOG("Failed to cast cardNumber in DRAW_CARD event. Actual TypeCode: %d", objCardNumber->getType());
            cocosUIListener->writeString(EG::JString(L"Failed to cast cardNumber in DRAW_CARD event."));
            return;
        }
    }
    else {
        CCLOG("cardNumber not found or incorrect TypeCode in DRAW_CARD event.");
        cocosUIListener->writeString(EG::JString(L"cardNumber not found or incorrect TypeCode in DRAW_CARD event."));
        return;
    }

    // 确定目标玩家
    players::Player* targetPlayer = nullptr;
    bool isLocalPlayer = false;
    if (playerNumber == 1) {
        targetPlayer = player1;
        isLocalPlayer = (localPlayerNumber == 1);
    }
    else if (playerNumber == 2) {
        targetPlayer = player2;
        isLocalPlayer = (localPlayerNumber == 2);
    }
    else {
        CCLOG("Invalid playerNumber in DRAW_CARD event: %d", playerNumber);
        cocosUIListener->writeString(EG::JString(L"Invalid playerNumber in DRAW_CARD event: ") +
            EG::JString(std::to_wstring(playerNumber).c_str()));
        return;
    }

    // 将卡牌分配给玩家
    targetPlayer->addCardToHand(cardNumber);

    if (isLocalPlayer) {
        // 为本地玩家创建卡牌精灵
        auto newCard = Sprite::create("cardfortest.png"); // 使用卡牌正面纹理
        if (newCard) {
            newCard->setTag(cardNumber); // 使用 cardNumber 作为 tag

            // 设置初始位置（统一为本地玩家的位置）
            Size visibleSize = Director::getInstance()->getVisibleSize();
            Vec2 origin = Director::getInstance()->getVisibleOrigin();
            int yPos = (localPlayerNumber == 1) ? CARD_REGION_Y : (visibleSize.height - CARD_REGION_Y); // 根据玩家编号设置Y位置

            Vec2 originalPos = Vec2(CARD_REGION_X + (targetPlayer->getHand().size() - 1) * (newCard->getContentSize().width + 30), yPos);

            // 添加到场景
            this->addChild(newCard);
            CCLOG("Added cardNumber: %d to scene. Card pointer: %p", cardNumber, newCard);

            // 添加到 localPlayerCards 并记录位置
            cardOriginalPositions[newCard] = originalPos;
            localPlayerCards.push_back(newCard);

            // 设置卡牌位置和缩放
            newCard->setPosition(originalPos);
            newCard->setScale(1.0f);

            // 运行动画
            newCard->runAction(Sequence::create(
                EaseOut::create(MoveBy::create(0.2f, Vec2(0, 50)), 2.0f),
                Spawn::create(
                    EaseInOut::create(MoveTo::create(0.5f, originalPos), 2.0f),
                    EaseInOut::create(ScaleTo::create(0.5f, 1.0f), 2.0f),
                    RotateBy::create(0.5f, 360), // 旋转一圈
                    nullptr
                ),
                EaseElasticOut::create(ScaleTo::create(0.3f, 1.0f)),
                nullptr
            ));

            // 播放抽牌音效
            audioPlayer("Music/drawcard.mp3", false);

            // 更新UI
            updatePlayerUI();

            // Log
            cocosUIListener->writeString(EG::JString(L"Handled DRAW_CARD event for cardNumber: ") +
                EG::JString(std::to_wstring(cardNumber).c_str()) +
                EG::JString(L" by playerNumber: ") +
                EG::JString(std::to_wstring(playerNumber).c_str()));
        }
        else {
            CCLOG("Failed to create card sprite for cardNumber: %d", cardNumber);
            cocosUIListener->writeString(EG::JString(L"Failed to create card sprite for cardNumber: ") +
                EG::JString(std::to_wstring(cardNumber).c_str()));
        }
    }
    else {
        // 对手玩家抽牌，不创建卡牌精灵
        // 仅更新对手玩家的手牌数据，不在UI上显示
        CCLOG("Opponent player %d drew a card. Not creating card sprite.", playerNumber);
        cocosUIListener->writeString(EG::JString(L"Opponent drew a card."));
        // 如果需要，可以在UI上显示对手手牌数量的变化
        // 例如，更新一个标签显示对手的手牌数量
    }
}

// 分发初始手牌
void BoardScene::distributeInitialHands()
{
    // 为本地玩家抽取3张卡牌
    for (int i = 0; i < 3; ++i) {
        players::Player* localPlayer = (localPlayerNumber == 1) ? player1 : player2;
        auto cardNumber = localPlayer->drawCard();
        if (cardNumber != -1) {
            sendDrawCardEvent(localPlayer->getPlayerNumber(), cardNumber);
            CCLOG("Player %d drew initial cardNumber: %d", localPlayer->getPlayerNumber(), cardNumber);
            cocosUIListener->writeString(EG::JString(L"Player drew initial card."));
        }
    }
}


// 辅助方法：根据卡牌ID查找精灵
Sprite* BoardScene::findCardByID(int cardNumber) {
    for (auto& cardSprite : localPlayerCards) {
        if (cardSprite->getTag() == cardNumber) {
            return cardSprite;
        }
    }

    CCLOG("Local Player's card with cardNumber %d not found.", cardNumber);
    return nullptr;
}


// 处理游戏结束
void BoardScene::endGame(players::Player* winner) {
    // 显示游戏结束界面
    std::string result = winner->getNickname() + " Wins!";
    auto resultLabel = Label::createWithTTF(result, "fonts/arial.ttf", 48);
    resultLabel->setPosition(Director::getInstance()->getVisibleSize() / 2);
    this->addChild(resultLabel, 10);

    // 停止游戏中的所有操作
    // 例如，禁用按钮，停止计时等
    cocosUIListener->writeString(EG::JString(L"Game Over: ") +
        EG::JString(result.c_str()));
}

// 更新逻辑
void BoardScene::update(float dt) {
    // 更新PhotonLib
    if (photonLib) {
        photonLib->update();
    }
}

// 析构函数
BoardScene::~BoardScene()
{
    // 清理卡牌精灵
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

    // 清理卡牌位置记录
    cardOriginalPositions.clear();

    player1 = nullptr;
    player2 = nullptr;
    CCLOG("BoardScene destroyed and all card references cleared.");
}
