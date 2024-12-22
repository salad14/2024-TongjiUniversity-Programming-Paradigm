/****************************************************************
 * 游戏卡组编辑场景的实现
 * Author: Lee
 ****************************************************************/
#include "EditGroupScene.h"
#include "MainScene.h"
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"

USING_NS_CC;
using namespace cocos2d::ui;  // 使用 UI 命名空间

Scene* EditGroupScene::createScene() {
    return EditGroupScene::create();
}

bool EditGroupScene::init() {
    if (!Scene::init()) {
        return false;
    }

    // 加载背景
    auto background = Sprite::create("Scenes/EditGroupScene.png");
    background->setPosition(Vec2(1024, 640));  // 2048/2, 1280/2
    this->addChild(background);

    // 初始化各个部分
    initScrollView();
    initSelectedArea();
    initButtons();
    //initDragAndDrop();

    return true;
}

void EditGroupScene::initScrollView() {
    // 创建滚动视图容器
    m_cardContainer = Node::create();
    // 计算布局参数
    const float cardWidth = 180;
    const float cardHeight = 248;
    const float spacing = 10;

    // 计算内容区域的总高度
    int totalRows = (50 + CARD_PER_ROW - 1) / CARD_PER_ROW;  // 向上取整
    float contentHeight = totalRows * (cardHeight + spacing) + spacing;

    // 放置50张卡牌，从上往下排列
    for (int i = 0; i < 50; i++) {
        auto card = Sprite::create("cards/card.png");
        card->setScale(0.8f);
        float x = (i % CARD_PER_ROW) * (cardWidth + spacing) + cardWidth / 2 + spacing;
        float y = contentHeight - ((i / CARD_PER_ROW) * (cardHeight + spacing) + cardHeight / 2 + spacing);
        card->setPosition(Vec2(x, y));
        card->setTag(i + 1);  // 设置卡牌id为1-50
        // 为每张卡牌添加点击监听器
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [this, card](Touch* touch, Event* event) {
            auto target = static_cast<Sprite*>(event->getCurrentTarget());
            Vec2 locationInNode = target->convertToNodeSpace(touch->getLocation());
            Size s = target->getContentSize();
            Rect rect = Rect(0, 0, s.width, s.height);
            // 如果点击在卡牌范围内
            if (rect.containsPoint(locationInNode) && m_selectedCardIds.size() < 30) {
                CCLOG("Card clicked: %d", target->getTag());
                onCardSelected(target->getTag());
                return true;
            }
            return false;
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, card);
        m_cardContainer->addChild(card);
    }
    // 创建滚动视图
    m_scrollView = ui::ScrollView::create();
    m_scrollView->setContentSize(cocos2d::Size(EDIT_SELECT_WIDTH, EDIT_SELECT_HEIGHT));
    m_scrollView->setInnerContainerSize(cocos2d::Size(EDIT_SELECT_WIDTH, contentHeight)); // 使用计算出的内容高度
    m_scrollView->setPosition(Vec2(EDIT_START_X, EDIT_START_Y));
    m_scrollView->setScrollBarEnabled(true);
    m_scrollView->setDirection(ui::ScrollView::Direction::VERTICAL);
    m_scrollView->setBounceEnabled(true);
    m_scrollView->setTouchEnabled(true);
    m_scrollView->addChild(m_cardContainer);
    // 添加鼠标滚轮事件监听
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseScroll = [this](Event* event) {
        auto mouseEvent = static_cast<EventMouse*>(event);
        float scrollY = mouseEvent->getScrollY() * 20; // 调整滚动速度
        float currentY = m_scrollView->getInnerContainer()->getPositionY();
        float minY = m_scrollView->getContentSize().height - m_scrollView->getInnerContainerSize().height;
        float maxY = 0;
        float newY = std::min(maxY, std::max(minY, currentY + scrollY));
        m_scrollView->getInnerContainer()->setPositionY(newY);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
    this->addChild(m_scrollView);
}

void EditGroupScene::initSelectedArea() {
    m_selectedContainer = Node::create();
    m_selectedContainer->setPosition(Vec2(EDIT_SELECTED_X, EDIT_SELECTED_Y));
    // 创建背景框
    auto selectedBg = DrawNode::create();
    selectedBg->drawSolidRect(
        Vec2(0, 0),
        Vec2(EDIT_SELECTED_WIDTH, EDIT_SELECTED_HEIGHT),
        Color4F(0.2f, 0.2f, 0.2f, 0.3f)  // 半透明灰色背景
    );
    m_selectedContainer->addChild(selectedBg);
    this->addChild(m_selectedContainer);
}

void EditGroupScene::onCardSelected(int cardId) {
    // 检查是否已经选择了这张卡
    if (std::find(m_selectedCardIds.begin(), m_selectedCardIds.end(), cardId)
        != m_selectedCardIds.end()) {
        return;
    }
    m_selectedCardIds.push_back(cardId);
    // 在右侧添加小图标
    auto card = Sprite::create("cards/card.png");
    card->setScale(0.4f);
    // 计算位置（每行5个）
    int index = m_selectedCardIds.size() - 1;
    float cardSpacing = 100;
    float startX = 50;
    float startY = EDIT_SELECTED_HEIGHT - 100;
    float x = startX + (index % 5) * cardSpacing;
    float y = startY - (index / 5) * cardSpacing;
    card->setPosition(Vec2(x, y));
    card->setTag(cardId);
    // 为右侧卡牌添加点击监听器
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this, card](Touch* touch, Event* event) {
        auto target = static_cast<Sprite*>(event->getCurrentTarget());
        Vec2 locationInNode = target->convertToNodeSpace(touch->getLocation());
        Size s = target->getContentSize();
        Rect rect = Rect(0, 0, s.width, s.height);
        if (rect.containsPoint(locationInNode)) {
            
            onCardRemoved(target->getTag());
            target->removeFromParent();
            return true;
        }
        return false;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, card);
    m_selectedContainer->addChild(card);
    // 播放选择音效
    audioPlayer("Music/ClickSoundEffect.mp3", false);
}

void EditGroupScene::onCardRemoved(int cardId) {
    auto it = std::find(m_selectedCardIds.begin(), m_selectedCardIds.end(), cardId);
    if (it != m_selectedCardIds.end()) {
        // 从已选卡组中移除
        m_selectedCardIds.erase(it);
        // 获取所有卡牌精灵并重新排列
        std::vector<Sprite*> cards;
        for (auto child : m_selectedContainer->getChildren()) {
            // 跳过背景节点（背景节点的 tag 为0）
            if (child->getTag() > 0) {
                cards.push_back(static_cast<Sprite*>(child));
            }
        }
        // 重新设置所有卡牌的位置
        for (size_t i = 0; i < cards.size(); i++) {
            float x = 50 + (i % 5) * 100;  // 每行5张，间距100
            float y = EDIT_SELECTED_HEIGHT - 100 - (i / 5) * 100;  // 从上往下排列

            // 使用动画移动到新位置
            cards[i]->runAction(MoveTo::create(0.2f, Vec2(x, y)));
        }
        // 播放取消选择音效
        audioPlayer("Music/ClickSoundEffect.mp3", false);
    }
}

void EditGroupScene::initButtons() {
    // 创建完成按钮
    auto finish = MenuItemImage::create(
        "button/Finish.png",
        "button/FinishSelected.png",
        CC_CALLBACK_1(EditGroupScene::onFinishButtonClicked, this)
    );
    finish->setPosition(Vec2(1800, 100));

    auto menu = Menu::create(finish, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
}

void EditGroupScene::onFinishButtonClicked(Ref* sender) {
    // 播放点击音效
    audioPlayer("Music/ClickSoundEffect.mp3", false);

    // 保存卡组信息
    if (m_selectedCardIds.size() == 30) {
        // 使用 UserDefault 保存卡组信息
        auto userDefault = UserDefault::getInstance();
        for (int i = 0; i < 30; i++) {
            std::string key = "customDeck_" + std::to_string(i);
            userDefault->setIntegerForKey(key.c_str(), m_selectedCardIds[i]);
        }
        userDefault->flush();

        // 返回主菜单
        Director::getInstance()->popScene();
    }
    else {
        // 显示提示信息：卡组必须包含30张卡牌
        auto label = Label::createWithTTF("卡组必须包含30张卡牌！", "fonts/arial.ttf", 24);
        label->setPosition(Vec2(1024, 640));
        label->setColor(Color3B::RED);
        this->addChild(label);
        label->runAction(Sequence::create(
            FadeIn::create(0.5f),
            DelayTime::create(1.0f),
            FadeOut::create(0.5f),
            RemoveSelf::create(),
            nullptr
        ));
        // 返回主菜单
        Director::getInstance()->popScene();
    }
}