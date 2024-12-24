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
    localMinionCard.clear();
    oppentMinionCard.clear();

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
    cocosUIListener->attachToLayer(uiLayer, Vec2(100, 100));// 根据需要调整位置

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

// 添加卡牌的属性标签
void BoardScene::addCardStats(cardSprite* newcard) {
    if (!newcard) return;
    // auto minionCard = std::dynamic_pointer_cast<MinionCard>(newcard->card);
    // if (!minionCard) throw std::runtime_error("Attacker is not a minion card");

    // 创建并添加属性标签
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

// 更新卡牌上的属性标签
void BoardScene::updateCardStats(cardSprite* card) {
    if (!card) return;
    // 检查卡牌是否存在于 cardStats 中
    auto healthLabel = dynamic_cast<Label*>(card->getChildByName("healthLabel"));
    auto attackLabel = dynamic_cast<Label*>(card->getChildByName("attackLabel"));
    //auto costLabel = dynamic_cast<Label*>(newcard->getChildByName("costLabel"));
    // 更新标签
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

// 返回主菜单
void BoardScene::cancelCallback(Ref* pSender) {
    // 加载点击音效
    audioPlayer("Music/ClickSoundEffect.mp3", false);
    // 离开房间
    photonLib->leaveRoom();
    cocosUIListener->writeString(EG::JString(L"Leaving room and returning to main menu."));
    // 切换到主菜单
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene(), Color3B::WHITE));
}

// 创建中央出牌区域
void BoardScene::createDropArea() {
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
void BoardScene::attackmove(PlayerNumber player, int attackerIndex, int defenderIndex) {
    if (attackerIndex < 0 || defenderIndex < 0) { return; }

    cardSprite* attacker = nullptr;
    cardSprite* defender = nullptr;
    if (player == localPlayerNumber) {
        if (attackerIndex >= localMinionCard.size() || defenderIndex >= oppentMinionCard.size())
            throw std::runtime_error("out of index");
        attacker = localMinionCard[attackerIndex];
        defender = oppentMinionCard[defenderIndex];
    }
    else {
        if (defenderIndex >= localMinionCard.size() || attackerIndex >= oppentMinionCard.size())
            throw std::runtime_error("out of index");
        attacker = oppentMinionCard[defenderIndex];
        defender = localMinionCard[attackerIndex];
    }

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

// 处理对英雄的攻击
void BoardScene::handleMinionAttackHero() {
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
void BoardScene::removeCardWithAnimation(cardSprite* card) {
    if (!card) return;
    // 先从 cardStats 中移除
    // cardStats.erase(card);

    // 卡牌消失动画
    card->runAction(Sequence::create(
        Spawn::create(
            FadeOut::create(0.5f),
            ScaleTo::create(0.5f, 0.1f),
            nullptr
        ),
        CallFunc::create([this, card]() {
            // 从数组中移除
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

    // 播放移除音效
    audioPlayer("Music/broken.mp3", false);
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
            scaleSprite(newHoveredSprite, 1.5f);// 放大1.5倍
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

    // 如果已经选中了攻击随从，检查是否点击了有效目标
    if (attackingCard) {
        // 检查是否点击了敌方随从或敌方英雄区域
        bool targetFound = false;

        // 检查敌方随从
        for (int i = 0; i < oppentMinionCard.size(); ++i) {
            cardSprite* enemyCard = oppentMinionCard[i];
            if (enemyCard->getBoundingBox().containsPoint(touchLocation)) {
                sendMinionAttackEvent(localPlayerNumber, get_localMinionIndex(attackingCard), i);
                targetFound = true;
                break;
            }
        }

        // 检查敌方英雄区域
        if (!targetFound && touchLocation.y > Director::getInstance()->getVisibleSize().height * 0.7f) {
            sendMinionAttackEvent(localPlayerNumber, get_localMinionIndex(attackingCard), -1);
        }

        // 清除攻击状态
        attackingCard = nullptr;
        if (attackIndicator) {
            attackIndicator->removeFromParent();
            attackIndicator = nullptr;
        }
        return true;
    }

    // 是否点击本方随从
    for (auto card : localMinionCard) {
        if (card->getBoundingBox().containsPoint(touchLocation)) {
            attackingCard = card;
            createAttackIndicator(card->getPosition());
            return true;
        }
    }

    // 检测手牌
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
        selectedCard = nullptr;   // 先清除选中状态

        if (inDropArea) {
            // 在出牌区域内，执行移除
            players::Player* currentPlayer = (currentPlayerNumber == 1) ? player1 : player2;
            if (currentPlayer) {
                // 获取卡牌费用
                int cardCost = jsonmanager.getCardCost(cardToHandle->card->dbfId);
                if (currentPlayer->getMoney() >= cardCost) {// 使用 getter 方法
                    // 扣除法力值
                    currentPlayer->setMoney(currentPlayer->getMoney() - cardCost);
                    CCLOG("Player %d mana reduced to %d", localPlayerNumber, currentPlayer->getMoney());

                    // 更新UI
                    updatePlayerUI();

                    //处理随从牌类
                    if (cardToHandle->card->type == cardType::MINION) {
                        //发送打出随从牌事件    
                        sendPlay_MinionCardEvent(localPlayerNumber, cardToHandle->card->dbfId);
                        // 显示卡牌在战场上
                        add_HandCardToBattlefield(cardToHandle);
                        // 从手牌中移除卡牌
                        removeCard(cardToHandle);
                    }
                    // 处理法术牌类
                    else if (cardToHandle->card->type == cardType::SPELL) {
                        sendPlay_SpellCardEvent(localPlayerNumber, cardToHandle->card->dbfId);
                        auto spell = dynamic_pointer_cast<SpellCard>(cardToHandle->card);
                        if (spell->hasMechanic(SpellMechanics::KeyWord::Damage)) {
                            // 触发伤害信号
                            //
                        }
                        else if (spell->hasMechanic(SpellMechanics::KeyWord::Draw)) {
                            // 收一张牌
                            //
                        }
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

// 将卡牌放回手牌
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
    turnIndicator->setPosition(Vec2(1880, 600));
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

    //更新回合指示
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
        // sprite->removeFromParent();
        // 播放音效
        audioPlayer("Music/putcard.mp3", false);

        // 更新剩余卡牌的位置
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

// 更新己方随从的位置
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

// 更新敌人的随从位置
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

// 发送打随从牌事件
void BoardScene::sendPlay_MinionCardEvent(PlayerNumber playerNumber, CardNumber dbfID) {
    EG::Hashtable eventContent;
    // 使用不同的 key 来区分 playerNumber 和 cardNumber
    eventContent.put(static_cast<unsigned char>(0), EG::Helpers::ValueToObject<EG::Object>::get(playerNumber));
    eventContent.put(static_cast<unsigned char>(1), EG::Helpers::ValueToObject<EG::Object>::get(dbfID));

    photonLib->raiseCustomEvent(eventContent, PLAY_MINION_CARD, ExitGames::Lite::ReceiverGroup::OTHERS); // ??????ж?????????
    CCLOG("Sent PLAY_MINION_CARD event with playerNumber: %d, cardID: %d", playerNumber, dbfID);
    cocosUIListener->writeString(EG::JString(L"Sent PLAY_MinionCard event with playerNumber: ") +
        EG::JString(std::to_wstring(playerNumber).c_str()) +
        EG::JString(L", cardID: ") +
        EG::JString(std::to_wstring(dbfID).c_str()));
}

// 发送打法术牌事件
void BoardScene::sendPlay_SpellCardEvent(PlayerNumber playerNumber, CardNumber dbfID) {
    EG::Hashtable eventContent;
    // 使用不同的 key 来区分 playerNumber 和 cardNumber
    eventContent.put(static_cast<unsigned char>(0), EG::Helpers::ValueToObject<EG::Object>::get(playerNumber));
    eventContent.put(static_cast<unsigned char>(1), EG::Helpers::ValueToObject<EG::Object>::get(dbfID));


    photonLib->raiseCustomEvent(eventContent, PLAY_SPELL_CARD, ExitGames::Lite::ReceiverGroup::OTHERS);
    CCLOG("Sent PLAY_SPELL_CARD event with playerNumber: %d, cardID: %d", playerNumber, dbfID);
    cocosUIListener->writeString(EG::JString(L"Sent PLAY_SpellCard event with playerNumber: ") +
        EG::JString(std::to_wstring(playerNumber).c_str()) +
        EG::JString(L", cardID: ") +
        EG::JString(std::to_wstring(dbfID).c_str()));
}

// 发送随从攻击事件
void BoardScene::sendMinionAttackEvent(PlayerNumber playerNumber, int attackerIndex, int defenderIndex) {
    EG::Hashtable eventContent;
    // 根据不同的 key 区分发出者和接收者
    eventContent.put(static_cast<unsigned char>(0), EG::Helpers::ValueToObject<EG::Object>::get(playerNumber));
    eventContent.put(static_cast<unsigned char>(1), EG::Helpers::ValueToObject<EG::Object>::get(attackerIndex));
    eventContent.put(static_cast<unsigned char>(2), EG::Helpers::ValueToObject<EG::Object>::get(defenderIndex));


    photonLib->raiseCustomEvent(eventContent, MINION_ATTACK, ExitGames::Lite::ReceiverGroup::ALL);
    CCLOG("Sent MINION_ATTACK_EVENT with playerNumber: %d, attackerIndex: %d, defenderIndex: %d", playerNumber, attackerIndex, defenderIndex);
    cocosUIListener->writeString(EG::JString(L"Sent MINION_ATTACK_EVENT with playerNumber: ") +
        EG::JString(std::to_wstring(playerNumber).c_str()) +
        EG::JString(L", attackerIndex: ") +
        EG::JString(std::to_wstring(attackerIndex).c_str()) +
        EG::JString(L", defenderIndex: ") +
        EG::JString(std::to_wstring(defenderIndex).c_str()));
}

// 发送造成伤害的法术牌事件
void BoardScene::sendSpellAttackEvent(PlayerNumber attackPlayer, int defenderIndex, int damage)
{
    EG::Hashtable eventContent;
    // 根据不同的 key 区分发出者和接收者
    eventContent.put(static_cast<unsigned char>(0), EG::Helpers::ValueToObject<EG::Object>::get(attackPlayer));
    eventContent.put(static_cast<unsigned char>(1), EG::Helpers::ValueToObject<EG::Object>::get(defenderIndex));
    eventContent.put(static_cast<unsigned char>(2), EG::Helpers::ValueToObject<EG::Object>::get(damage));

    photonLib->raiseCustomEvent(eventContent, SPELL_ATTACK, ExitGames::Lite::ReceiverGroup::ALL);

    CCLOG("Sent SPELL_ATTACK_EVENT with attackPlayer: %d, defenderIndex: %d, damage: %d", attackPlayer, defenderIndex, damage);
    cocosUIListener->writeString(EG::JString(L"Sent SPELL_ATTACK_EVENT with attackPlayer: ") +
        EG::JString(std::to_wstring(attackPlayer).c_str()) +
        EG::JString(L", defenderIndex: ") +
        EG::JString(std::to_wstring(defenderIndex).c_str()) +
        EG::JString(L", damage: ") +
        EG::JString(std::to_wstring(damage).c_str()));
}

// 处理 Photon 自定义事件
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
            handle_SpellAttackEvent(parameters);
            break;
        case MINION_ATTACK:
            handle_MinionAttackEvent(parameters);
            break;
        default:
            CCLOG("Received unknown eventCode: %d", eventCode);
            cocosUIListener->writeString(EG::JString(L"Received unknown eventCode: ") +
                EG::JString(std::to_wstring(eventCode).c_str()));
            break;
    }
}

// 处理打牌事件
void BoardScene::handle_PlayMinionCard(const EG::Hashtable& parameters) {
    int playerNumber = 0;
    int dbfId = 0;

    // 获取 playerNumber
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

    // 获取 cardNumber
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

    // 判断是否与本地玩家编号相同
    if (playerNumber == localPlayerNumber) return;

    // 确定目标玩家
    players::Player* targetPlayer = nullptr;
    if (playerNumber == 1) {
        targetPlayer = player1;
    }
    else if (playerNumber == 2) {
        targetPlayer = player2;
    }
    // 获取法力值消耗
    int cardCost = jsonmanager.getCardCost(dbfId);
    if (targetPlayer->getMoney() < cardCost) {
        CCLOG("Player %d does not have enough mana to play card %d.", playerNumber, dbfId);
        cocosUIListener->writeString(EG::JString(L"Player does not have enough mana to play the card."));
        return;
    }
    targetPlayer->setMoney(targetPlayer->getMoney() - cardCost);
    CCLOG("Player %d mana reduced to %d", playerNumber, targetPlayer->getMoney());

    // 更新UI
    updatePlayerUI();

    if (playerNumber != localPlayerNumber) {
        // 显示对方玩家添加到战场上的卡牌
        add_NewCardToBattlefield(playerNumber, dbfId);
    }

    // ??????????????????????????????? // ??????????????????? ?????????
    //if (playerNumber == localPlayerNumber) {
    //    cardSprite* card = findCardByID(cardNumber);
    //    if (card) {
    //        removeCard(card);
    //    }
    //}
}

// 法术牌打出处理逻辑
void BoardScene::handle_PlaySpellCard(const EG::Hashtable& parameters) {
    int playerNumber = 0;
    int dbfId = 0;

    // 打出法术牌的玩家id：playerNumber
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

    // 打出法术牌的 cardNumber
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

    // 判定是否是本地玩家
    if (playerNumber == localPlayerNumber) return;

    // 更新UI
    // ShowOpponentPlayedCard(dbfId);
}

// 处理打出攻击类法术牌
void BoardScene::handle_SpellAttackEvent(const EG::Hashtable& parameters) {
    int playerNumber = 0;
    int defenderIndex = 0;
    int damage = 0;

    // 发playerNumber
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

    // 接收的defenderIndex
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

    // damage伤害量
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
    if (playerNumber == localPlayerNumber) { // 己方攻击
        if (defenderIndex == -1) { // 攻击英雄
            if (!targetplayer->getDamage(damage)) { // 英雄死亡
                endGame(localplayer);
                cocosUIListener->writeString(EG::JString(L"Hero is dead."));
            }
            updatePlayerUI();
            return;
        }
        else {
            defender = oppentMinionCard[defenderIndex];
            defender->getDamage(damage);
            checkMinionDie(defender);
            // updateCardStats(defender);
            // ????????????
            // .....
        }
    }
    else { // 敌方攻击
        if (defenderIndex == -1) { // 攻击英雄
            if (!localplayer->getDamage(damage)) { // 英雄死亡
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
            // updateCardStats(defender);
            // ????????????
            // .....
        }
    }
}

// 处理随从攻击事件
void BoardScene::handle_MinionAttackEvent(const ExitGames::Common::Hashtable& parameters) {
    int playerNumber = 0;
    int attackerIndex = -1;
    int defenderIndex = -1;

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
    if (playerNumber == localPlayerNumber) { // 己方随从攻击
        attacker = localMinionCard[attackerIndex];
        if (defenderIndex != -1)
            defender = oppentMinionCard[defenderIndex];
    }
    else {
        attacker = oppentMinionCard[attackerIndex];
        if (defenderIndex != -1)
            defender = localMinionCard[defenderIndex];
    }

    if (defenderIndex == -1) { // 攻击英雄
        auto targetplayer = (localPlayerNumber == 1) ? player2 : player1; // 敌方英雄
        auto localplayer = (localPlayerNumber == 1) ? player1 : player2;  // 己方英雄
        if (playerNumber == localPlayerNumber) { // 己方随从攻击对方英雄
            if (!targetplayer->getDamage(attacker->currentAttack)) {
                endGame(targetplayer);
                cocosUIListener->writeString(EG::JString(L"Hero is dead."));
                return;
            }
        }
        else { // 敌方随从攻击己方英雄
            if (!localplayer->getDamage(attacker->currentAttack)) {
                endGame(localplayer);
                cocosUIListener->writeString(EG::JString(L"Hero is dead."));
                return;
            }
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

    // 检查随从是否死亡
    checkMinionDie(attacker);
    checkMinionDie(defender);

    // 更新UI
    updatePlayerUI();

    // ?????????Ч
    audioPlayer("Music/attack.mp3", false);
}

// 将一张新的手牌加入到战场 // 对手出随从时调用
void BoardScene::add_NewCardToBattlefield(int playerNumber, int cardNumber) {
    // ??????????飬?????????? createWithCard ????
    auto newCard = jsonmanager.find_by_dbfId(cardNumber);
    if (!newCard) {
        CCLOG("Card data not found for cardNumber: %d", cardNumber);
        cocosUIListener->writeString(EG::JString(L"Card data not found for cardNumber: ") +
            EG::JString(std::to_wstring(cardNumber).c_str()));
        return;
    }
    Size desiredSize(200, 250); // ???????????
    // ???????????
    auto battlefieldCard = cardSprite::createWithCard(newCard, desiredSize); // ?????????????
    if (battlefieldCard) {
        battlefieldCard->setTag(cardNumber); // ??? cardNumber ??? tag

        auto minionCard = std::dynamic_pointer_cast<MinionCard>(newCard);
        if (!minionCard) throw std::runtime_error("newCard is not a minion card");

        battlefieldCard->currentAttack = minionCard.get()->attack;
        battlefieldCard->currentHealth = minionCard.get()->maxhealth;

        //if (playerNumber == localPlayerNumber) {
        //    // ????????????????
        //    //targetPos = Vec2(PLAYED_AREA_PLAYER1_X + localMinionCard.size() * CARD_SPACING, PLAYED_AREA_Y);
        //    localMinionCard.push_back(battlefieldCard);
        //    updatePlayedCardsPosition();
        //}
        //else {
        //    oppentMinionCard.push_back(battlefieldCard);
        //    updateEnemyCardsPosition();
        //}

        oppentMinionCard.push_back(battlefieldCard);
        addCardStats(battlefieldCard);
        updateCardStats(battlefieldCard);
        updateEnemyCardsPosition();

        // ?????????????λ??
        this->addChild(battlefieldCard, 50);

        //// ???????????
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

// 将自己手牌中的一张牌置入战场 // 己方出牌时调用
void BoardScene::add_HandCardToBattlefield(cardSprite* minion) {
    if (!minion) {
        CCLOG("Invalid card: minion is null");
        return;
    }

    auto minionCard = std::dynamic_pointer_cast<MinionCard>(minion->card);
    if (!minionCard) { throw std::runtime_error("Card is not a minion card"); return; }

    // ????????????????????
    minion->currentAttack = minionCard->attack;
    minion->currentHealth = minionCard->maxhealth;

    localMinionCard.push_back(minion);
    updateCardStats(minion);
    updatePlayedCardsPosition();
}

// 回合结束
void BoardScene::handle_TurnStart(const EG::Hashtable& parameters) {
    int receivedPlayerNumber = 0;

    // ??? receivedPlayerNumber
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

    // ??? receivedPlayerNumber ?????Ч
    if (receivedPlayerNumber != 1 && receivedPlayerNumber != 2) {
        CCLOG("Invalid receivedPlayerNumber: %d", receivedPlayerNumber);
        cocosUIListener->writeString(EG::JString(L"Invalid receivedPlayerNumber in TURN_START event."));
        return;
    }

    // ???????????????????
    currentPlayerNumber = receivedPlayerNumber;
    isLocalPlayerTurn = (currentPlayerNumber == localPlayerNumber);

    CCLOG("currentPlayerNumber set to: %d, isLocalPlayerTurn: %s", currentPlayerNumber, isLocalPlayerTurn ? "true" : "false");

    // ?????????????
    players::Player* currentPlayer = (currentPlayerNumber == 1) ? player1 : player2;
    currentPlayer->increaseMana();
    CCLOG("Player %d mana increased to %d", currentPlayerNumber, currentPlayer->getMoney());

    // ????UI
    updatePlayerUI();

    // ???????????????????????
    if (isLocalPlayerTurn) {
        if (currentPlayer->hasCards()) {
            std::shared_ptr<CardBase> card = currentPlayer->drawCard(); // ?????????
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

// 初始分配手牌
void BoardScene::distributeInitialHands()
{
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

// ???????????????????????????
void BoardScene::addCardToLocalPlayer(std::shared_ptr<CardBase> card) {
    players::Player* localPlayer = (localPlayerNumber == 1) ? player1 : player2;
    // localPlayer->addCardToHand(card);

    Size desiredSize(120, 180); // ??????????????????

    cardSprite* newCard = cardSprite::createWithCard(card, desiredSize);
    addCardStats(newCard);  // ???????????5????????1??????1
    if (newCard) {
        CCLOG("Successfully created cardSprite for cardNumber: %d", card->dbfId);
        newCard->setTag(card->dbfId); // ??? cardNumber ??? tag
        Vec2 originalPos(CARD_REGION_X + localPlayerCards.size() * (newCard->getContentSize().width + 30),
            CARD_REGION_Y);
        CCLOG("Setting card position to: (%f, %f)", originalPos.x, originalPos.y);

        // ??????????????????λ??
        this->addChild(newCard);
        cardOriginalPositions[newCard] = originalPos;
        localPlayerCards.push_back(newCard);

        // ???????????
        newCard->runAction(Sequence::create(
            // 1. ????????
            EaseOut::create(MoveBy::create(0.2f, Vec2(0, 50)), 2.0f),
            // 2. ????????λ?ò????
            Spawn::create(
                EaseInOut::create(MoveTo::create(0.5f, originalPos), 2.0f),
                EaseInOut::create(ScaleTo::create(0.5f, 1.0f), 2.0f),
                RotateBy::create(0.5f, 360), // ?????
                nullptr
            ),
            // 3. ??????????Ч??
            EaseElasticOut::create(ScaleTo::create(0.3f, 1.0f)),
            nullptr
        ));

        // ?????????Ч
        audioPlayer("Music/drawcard.mp3", false);

        // ????UI
        updatePlayerUI();

        // ???
        cocosUIListener->writeString(EG::JString(L"Added initial card locally."));
    }
    else {
        CCLOG("Failed to create card sprite for cardNumber: %d", card->dbfId);
        cocosUIListener->writeString(EG::JString(L"Failed to create card sprite for cardNumber: ") +
            EG::JString(std::to_wstring(card->dbfId).c_str()));
    }
}

// ?????????????????ID???????
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
        CCLOG("Minion die!  Releasing attacker with health %d", minion->currentHealth);
        // minion->removeFromParentAndCleanup(true);

        // ?????????????
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

int BoardScene::get_opponentMinionIndex(const cardSprite* minion) {
    for (size_t i = 0; i < localMinionCard.size(); ++i) {
        if (oppentMinionCard[i] == minion) return i;
    }
    throw std::runtime_error("Minion not found in opponentMinionCard");
    return -1;
}

// ???????????
void BoardScene::endGame(players::Player* winner) {
    std::string result;
    if (localPlayerNumber == 1) {
        if (winner->getNickname() == "player1")
            result = "You Win!";
        else result = "You Lose!";
    }
    else {
        if (winner->getNickname() == "player1")
            result = "You Lose!";
        else result = "You Win!";
    }


    // ??????????????
    // std::string result = winner->getNickname() + " Wins!";
    auto resultLabel = Label::createWithTTF(result, "fonts/arial.ttf", 48);
    resultLabel->setPosition(Director::getInstance()->getVisibleSize() / 2);
    this->addChild(resultLabel, 10);

    // ??????е????в???
    // ???磬???????????????
    cocosUIListener->writeString(EG::JString(L"Game Over: ") +
        EG::JString(result.c_str()));
}

// ???????
void BoardScene::update(float dt) {
    // ????PhotonLib
    if (photonLib) {
        photonLib->update();
    }
}

BoardScene::~BoardScene()
{
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (photonLib) {
        photonLib->setCustomEventCallback(nullptr);
    }
    // ???????????
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

    // ????????λ?ü??
    cardOriginalPositions.clear();

    player1 = nullptr;
    player2 = nullptr;
    CCLOG("BoardScene destroyed and all card references cleared.");
}

