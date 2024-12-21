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

    // 初始化战场卡牌容器
    localplayedCards.clear();
    oppentplayedCards.clear();

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
        cancel->setPosition(Vec2(1940, 80)); // 与第一次实现一致
    }
    auto menu = Menu::create(cancel, nullptr);

    // 添加菜单
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

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

    // 初始化玩家
    initPlayers();

    // 创建玩家信息UI
    createPlayerUI();

    this->scheduleUpdate();

    distributeInitialHands(); // 通过事件分发抽牌

    if (localPlayerNumber == 1)
    {
        sendTurnStartEvent();
    }
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
void BoardScene::scaleSprite(cardSprite* sprite, float scale) {
    // 使用动作实现平滑的缩放效果
    sprite->runAction(ScaleTo::create(0.1f, scale));
}

// 鼠标触摸检测
bool BoardScene::onTouchBegan(Touch* touch, Event* event)
{
    if (!isLocalPlayerTurn) {
        return false;
    }

    Vec2 touchLocation = touch->getLocation();

    // 从后向前检查（使最上层的精灵优先响应）
    for (auto it = localPlayerCards.rbegin(); it != localPlayerCards.rend(); ++it) {
        cardSprite* sprite = *it;
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

        cardSprite* cardToHandle = selectedCard;
        selectedCard = nullptr;  // 先清除选中状态

        if (inDropArea) {
            // 在出牌区域内，执行移除
            players::Player* currentPlayer = (currentPlayerNumber == 1) ? player1 : player2;
            if (currentPlayer) {
                // 获取卡牌费用
                // int cardNumber = cardToHandle->getTag();
                int cardCost = getCardCost(cardToHandle->card);
                if (currentPlayer->getMoney() >= cardCost) { // 使用 getter 方法
                    // 扣除法力值
                    currentPlayer->setMoney(currentPlayer->getMoney() - cardCost);
                    CCLOG("Player %d mana reduced to %d", localPlayerNumber, currentPlayer->getMoney());

                    // 更新UI
                    updatePlayerUI();

                    // 发送 PLAY_CARD 事件
                    /////////////////// need to be editted!!!
                    sendPlayCardEvent(localPlayerNumber, cardToHandle->card->dbfId);

                    // 显示卡牌在战场上
                    /////////////////// need to be editted!!!
                    addCardToBattlefield(localPlayerNumber, cardToHandle->card->dbfId);

                    // 从手牌中移除卡牌
                    removeCard(cardToHandle);
                }
                else
                {
                    // 费用不足，回到原位
                    Vec2 originalPos = cardOriginalPositions[cardToHandle];
                    cardToHandle->runAction(EaseBackOut::create(MoveTo::create(0.5f, originalPos)));
                }
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

// 获取卡牌费用
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

    // 根据 currentPlayerNumber 和 localPlayerNumber 设置 isLocalPlayerTurn
    isLocalPlayerTurn = (currentPlayerNumber == localPlayerNumber);
}

// 创建玩家信息UI
void BoardScene::createPlayerUI() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 定义本地玩家和对手的指针
    players::Player* localPlayer = (localPlayerNumber == 1) ? player1 : player2;
    players::Player* opponentPlayer = (localPlayerNumber == 1) ? player2 : player1;

    // 创建己方玩家信息显示
    localPlayerHealth = Label::createWithTTF("HP:" + std::to_string(localPlayer->getHealth()), "fonts/arial.ttf", 24);
    localPlayerHealth->setPosition(Vec2(800, 220));
    this->addChild(localPlayerHealth);
    // 设置描边（同时设置颜色和粗细）
    localPlayerHealth->enableOutline(Color4B::BLACK, 2);         // 黑色描边，粗细为2

    localPlayerMana = Label::createWithTTF("Mana:" + std::to_string(localPlayer->getMoney()) + "/" + std::to_string(localPlayer->getMaxMoney()), "fonts/arial.ttf", 24);
    localPlayerMana->setPosition(Vec2(800, 280));
    this->addChild(localPlayerMana);
    // 设置描边（同时设置颜色和粗细）
    localPlayerMana->enableOutline(Color4B::BLACK, 2);         // 黑色描边，粗细为2

    // 创建对方玩家信息显示
    opponentPlayerHealth = Label::createWithTTF("HP:" + std::to_string(opponentPlayer->getHealth()), "fonts/arial.ttf", 24);
    opponentPlayerHealth->setPosition(Vec2(800, visibleSize.height - 220));
    this->addChild(opponentPlayerHealth);
    opponentPlayerHealth->enableOutline(Color4B::BLACK, 2);         // 黑色描边，粗细为2

    opponentPlayerMana = Label::createWithTTF("Mana:" + std::to_string(opponentPlayer->getMoney()) + "/" + std::to_string(opponentPlayer->getMaxMoney()), "fonts/arial.ttf", 24);
    opponentPlayerMana->setPosition(Vec2(800, visibleSize.height - 280));
    this->addChild(opponentPlayerMana);
    opponentPlayerMana->enableOutline(Color4B::BLACK, 2);         // 黑色描边，粗细为2

    // 回合指示器
    turnIndicator = Label::createWithTTF("Your Turn", "fonts/arial.ttf", 32);
    if (isLocalPlayerTurn) {
        turnIndicator->setString("Your Turn");
    }
    else {
        turnIndicator->setString("Opponent's Turn");
    }
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

void BoardScene::updatePlayerUI() {
    // 定义本地玩家和对手的指针
    players::Player* localPlayer = (localPlayerNumber == 1) ? player1 : player2;
    players::Player* opponentPlayer = (localPlayerNumber == 1) ? player2 : player1;

    // 调试日志
    CCLOG("Updating UI:");
    CCLOG("Local Player (Number %d) - HP: %d, Mana: %d/%d", localPlayerNumber, localPlayer->getHealth(), localPlayer->getMoney(), localPlayer->getMaxMoney());
    CCLOG("Opponent Player (Number %d) - HP: %d, Mana: %d/%d",
        (localPlayerNumber == 1) ? 2 : 1,
        opponentPlayer->getHealth(),
        opponentPlayer->getMoney(),
        opponentPlayer->getMaxMoney());

    // 更新本地玩家生命值和法力值
    localPlayerHealth->setString("HP: " + std::to_string(localPlayer->getHealth()));
    localPlayerMana->setString("Mana: " + std::to_string(localPlayer->getMoney()) + "/" + std::to_string(localPlayer->getMaxMoney()));

    // 更新对手玩家生命值和法力值
    opponentPlayerHealth->setString("HP: " + std::to_string(opponentPlayer->getHealth()));
    opponentPlayerMana->setString("Mana: " + std::to_string(opponentPlayer->getMoney()) + "/" + std::to_string(opponentPlayer->getMaxMoney()));

    // 更新回合指示
    if (isLocalPlayerTurn) {
        turnIndicator->setString("Your Turn");
    }
    else {
        turnIndicator->setString("Opponent's Turn");
    }

    // 视觉上反映回合状态
    for (auto& card : localPlayerCards) {
        if (isLocalPlayerTurn) {
            // 可操作时，恢复正常外观
            card->setOpacity(255);
            card->setColor(Color3B::WHITE);
        }
        else {
            // 不可操作时，设置为半透明或灰色
            card->setOpacity(128); // 半透明
            card->setColor(Color3B(100, 100, 100)); // 灰色
        }
    }
}

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

    // 发送 TURN_START 事件
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
void BoardScene::removeCard(cardSprite* sprite) {
    if (!sprite) return;

    if (sprite == hoveredCard) {
        hoveredCard = nullptr;
    }

    // 从 localPlayerCards 移除
    auto iter = std::find(localPlayerCards.begin(), localPlayerCards.end(), sprite);
    if (iter != localPlayerCards.end()) {
        // 获取被移除卡牌的索引位置
        size_t removedIndex = std::distance(localPlayerCards.begin(), iter);

        localPlayerCards.erase(iter);
        cardOriginalPositions.erase(sprite);

        // 不再将卡牌添加到 playedCards，这里由 handlePlayCard 统一处理
        sprite->removeFromParent();
        // 播放音效
        audioPlayer("Music/putcard.mp3", false);

        // 更新剩余卡牌的位置
        Size visibleSize = Director::getInstance()->getVisibleSize();
        for (size_t i = removedIndex; i < localPlayerCards.size(); i++) {
            cardSprite* card = localPlayerCards[i];
            // 计算新位置
            Vec2 newPos;
            if (localPlayerNumber == 1) {
                newPos = Vec2(CARD_REGION_X + i * (card->getContentSize().width + 30), CARD_REGION_Y);
            }
            else {
                newPos = Vec2(visibleSize.width - CARD_REGION_X - card->getContentSize().width - i * (card->getContentSize().width + 30), CARD_REGION_Y);
            }

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

// 更新场上已打出卡牌的显示
void BoardScene::updatePlayedCardsPosition() {
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 center = Vec2(visibleSize.width / 2, visibleSize.height / 2);
    float cardWidth = 100;  // 假设卡牌宽度为100
    float spacing = 120;     // 卡牌间距

    // 本地玩家已打出的卡牌
    float startX_local = PLAYED_AREA_PLAYER1_X - (localplayedCards.size() * (cardWidth + spacing) - spacing) / 2;
    for (size_t i = 0; i < localplayedCards.size(); i++) {
        Sprite* card = localplayedCards[i];
        Vec2 targetPos = Vec2(startX_local + i * (cardWidth + spacing), PLAYED_AREA_Y + 100);

        // 使用动画移动到目标位置
        card->runAction(EaseBackOut::create(MoveTo::create(0.3f, targetPos)));
    }

    // 对手玩家已打出的卡牌
    float startX_opponent = PLAYED_AREA_PLAYER2_X - (oppentplayedCards.size() * (cardWidth + spacing) - spacing) / 2;
    for (size_t i = 0; i < oppentplayedCards.size(); i++) {
        Sprite* card = oppentplayedCards[i];
        Vec2 targetPos = Vec2(startX_opponent + i * (cardWidth + spacing), PLAYED_AREA_Y - 100);

        // 使用动画移动到目标位置
        card->runAction(EaseBackOut::create(MoveTo::create(0.3f, targetPos)));
    }
}


// 发送打牌事件
///////////////// to be editted
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

// 处理 Photon 自定义事件
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

// 处理打牌事件
void BoardScene::handlePlayCard(const EG::Hashtable& parameters) {
    int playerNumber = 0;
    int cardNumber = 0;

    // 获取 playerNumber
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

    // 获取 cardNumber
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

    // 确定目标玩家
    players::Player* targetPlayer = nullptr;
    if (playerNumber == 1) {
        targetPlayer = player1;
    }
    else if (playerNumber == 2) {
        targetPlayer = player2;
    }
    // 扣除目标玩家的法力值
    int cardCost = getCardCost(cardNumber);
    if (targetPlayer->getMoney() < cardCost) {
        CCLOG("Player %d does not have enough mana to play card %d.", playerNumber, cardNumber);
        cocosUIListener->writeString(EG::JString(L"Player does not have enough mana to play the card."));
        return;
    }
    targetPlayer->setMoney(targetPlayer->getMoney() - cardCost);
    CCLOG("Player %d mana reduced to %d", playerNumber, targetPlayer->getMoney());

    // 更新UI
    updatePlayerUI();

    // 显示打出的卡牌在战场上
    addCardToBattlefield(playerNumber, cardNumber);

    // 如果是本地玩家打出的卡牌，则从手牌中移除
    if (playerNumber == localPlayerNumber) {
        cardSprite* card = findCardByID(cardNumber);
        if (card) {
            removeCard(card);
        }
    }
}

// 添加卡牌到战场
void BoardScene::addCardToBattlefield(int playerNumber, int cardNumber) {
    JSONManager manager("cards/json/cards.json");

    // 创建卡牌精灵，使用修正后的 createWithCard 方法
    auto cardData = manager.find_by_dbfId(cardNumber);
    if (!cardData) {
        CCLOG("Card data not found for cardNumber: %d", cardNumber);
        cocosUIListener->writeString(EG::JString(L"Card data not found for cardNumber: ") +
            EG::JString(std::to_wstring(cardNumber).c_str()));
        return;
    }
    Size desiredSize(100, 150); // 根据需要调整
    // 创建卡牌精灵
    auto battlefieldCard = cardSprite::createWithCard(cardData, desiredSize); // 使用修正后的方法
    if (battlefieldCard) {
        battlefieldCard->setTag(cardNumber); // 使用 cardNumber 作为 tag

        // 设置战场上卡牌的位置
        Vec2 targetPos;

        if (playerNumber == localPlayerNumber) {
            // 本地玩家已打出的卡牌
            targetPos = Vec2(PLAYED_AREA_PLAYER1_X + localplayedCards.size() * CARD_SPACING, PLAYED_AREA_Y);
            localplayedCards.push_back(battlefieldCard);
        }
        else {
            // 对手玩家已打出的卡牌
            targetPos = Vec2(PLAYED_AREA_PLAYER2_X + oppentplayedCards.size() * CARD_SPACING, PLAYED_AREA_Y + 200);
            oppentplayedCards.push_back(battlefieldCard);
        }

        // 添加到战场并记录位置
        this->addChild(battlefieldCard, 1);

        // 设置初始位置在目标位置上方并淡入
        if (playerNumber == localPlayerNumber) {
            battlefieldCard->setPosition(Vec2(targetPos.x, targetPos.y + 100));
        }
        else {
            battlefieldCard->setPosition(Vec2(targetPos.x, targetPos.y - 100));
        }
        battlefieldCard->setOpacity(0);

        // 动画显示卡牌
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

    // 验证 receivedPlayerNumber 是否有效
    if (receivedPlayerNumber != 1 && receivedPlayerNumber != 2) {
        CCLOG("Invalid receivedPlayerNumber: %d", receivedPlayerNumber);
        cocosUIListener->writeString(EG::JString(L"Invalid receivedPlayerNumber in TURN_START event."));
        return;
    }

    // 更新当前玩家编号和本地回合状态
    currentPlayerNumber = receivedPlayerNumber;
    isLocalPlayerTurn = (currentPlayerNumber == localPlayerNumber);

    CCLOG("currentPlayerNumber set to: %d, isLocalPlayerTurn: %s", currentPlayerNumber, isLocalPlayerTurn ? "true" : "false");

    // 确定当前回合的玩家
    players::Player* currentPlayer = (currentPlayerNumber == 1) ? player1 : player2;
    currentPlayer->increaseMana();
    CCLOG("Player %d mana increased to %d", currentPlayerNumber, currentPlayer->getMoney());

    // 更新UI
    updatePlayerUI();

    // 如果是本地玩家的回合，抽一张卡牌
    if (isLocalPlayerTurn) {
        if (currentPlayer->hasCards()) {
            std::shared_ptr<CardBase> card = currentPlayer->drawCard(); // 抽取一张卡牌
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

// 分发初始手牌
void BoardScene::distributeInitialHands()
{
    // 为本地玩家抽取初始手牌
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

// 添加卡牌到本地玩家（仅本地显示）
void BoardScene::addCardToLocalPlayer(std::shared_ptr<CardBase> card) {
    players::Player* localPlayer = (localPlayerNumber == 1) ? player1 : player2;
    // localPlayer->addCardToHand(card);

    Size desiredSize(100, 150);

    cardSprite* newCard = cardSprite::createWithCard(card, desiredSize);

    if (newCard) {
        CCLOG("Successfully created cardSprite for cardNumber: %d", card->dbfId);
        newCard->setTag(card->dbfId); // 使用 cardNumber 作为 tag

        //float xPos = CARD_REGION_X + localPlayerCards.size() * (newCard->getContentSize().width + 30);
        Vec2 originalPos(CARD_REGION_X + localPlayerCards.size() * (newCard->getContentSize().width + 30),
            CARD_REGION_Y);
        //Vec2 originalPos(xPos, CARD_REGION_Y);
        CCLOG("Setting card position to: (%f, %f)", originalPos.x, originalPos.y);

        // 添加到待出牌区域并记录位置
        this->addChild(newCard);
        cardOriginalPositions[newCard] = originalPos;
        localPlayerCards.push_back(newCard);

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

        // 更新UI
        updatePlayerUI();

        // 日志
        cocosUIListener->writeString(EG::JString(L"Added initial card locally."));
    }
    else {
        CCLOG("Failed to create card sprite for cardNumber: %d", card->dbfId);
        cocosUIListener->writeString(EG::JString(L"Failed to create card sprite for cardNumber: ") +
            EG::JString(std::to_wstring(card->dbfId).c_str()));
    }
}


// 辅助方法：根据卡牌ID查找精灵
cardSprite* BoardScene::findCardByID(int cardNumber) {
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
    // 清理 PhotonLib 的自定义事件回调
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (photonLib) {
        photonLib->setCustomEventCallback(nullptr);
    }
    // 清理卡牌精灵
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

    // 清理卡牌位置记录
    cardOriginalPositions.clear();

    player1 = nullptr;
    player2 = nullptr;
    CCLOG("BoardScene destroyed and all card references cleared.");
}


///目前问题 UI不适配，原来的卡打出仍然存在，同时没能正确的实现对手卡牌的显示