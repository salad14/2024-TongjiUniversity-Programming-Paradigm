/****************************************************************
 * ��Ϸ����༭������ʵ��
 * Author: Lee
 ****************************************************************/
#include "EditGroupScene.h"
#include "MainScene.h"
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"

USING_NS_CC;
using namespace cocos2d::ui;  // ʹ�� UI �����ռ�

Scene* EditGroupScene::createScene() {
    return EditGroupScene::create();
}

bool EditGroupScene::init() {
    if (!Scene::init()) {
        return false;
    }

    // ���ر���
    auto background = Sprite::create("Scenes/EditGroupScene.png");
    background->setPosition(Vec2(1024, 640));  // 2048/2, 1280/2
    this->addChild(background);

    // ��ʼ����������
    initScrollView();
    initSelectedArea();
    initButtons();
    //initDragAndDrop();

    return true;
}

void EditGroupScene::initScrollView() {
    // ����������ͼ����
    m_cardContainer = Node::create();
    // ���㲼�ֲ���
    const float cardWidth = 180;
    const float cardHeight = 248;
    const float spacing = 10;

    // ��������������ܸ߶�
    int totalRows = (50 + CARD_PER_ROW - 1) / CARD_PER_ROW;  // ����ȡ��
    float contentHeight = totalRows * (cardHeight + spacing) + spacing;

    // ����50�ſ��ƣ�������������
    for (int i = 0; i < 50; i++) {
        auto card = Sprite::create("cards/card.png");
        card->setScale(0.8f);
        float x = (i % CARD_PER_ROW) * (cardWidth + spacing) + cardWidth / 2 + spacing;
        float y = contentHeight - ((i / CARD_PER_ROW) * (cardHeight + spacing) + cardHeight / 2 + spacing);
        card->setPosition(Vec2(x, y));
        card->setTag(i + 1);  // ���ÿ���idΪ1-50
        // Ϊÿ�ſ�����ӵ��������
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [this, card](Touch* touch, Event* event) {
            auto target = static_cast<Sprite*>(event->getCurrentTarget());
            Vec2 locationInNode = target->convertToNodeSpace(touch->getLocation());
            Size s = target->getContentSize();
            Rect rect = Rect(0, 0, s.width, s.height);
            // �������ڿ��Ʒ�Χ��
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
    // ����������ͼ
    m_scrollView = ui::ScrollView::create();
    m_scrollView->setContentSize(cocos2d::Size(EDIT_SELECT_WIDTH, EDIT_SELECT_HEIGHT));
    m_scrollView->setInnerContainerSize(cocos2d::Size(EDIT_SELECT_WIDTH, contentHeight)); // ʹ�ü���������ݸ߶�
    m_scrollView->setPosition(Vec2(EDIT_START_X, EDIT_START_Y));
    m_scrollView->setScrollBarEnabled(true);
    m_scrollView->setDirection(ui::ScrollView::Direction::VERTICAL);
    m_scrollView->setBounceEnabled(true);
    m_scrollView->setTouchEnabled(true);
    m_scrollView->addChild(m_cardContainer);
    // ����������¼�����
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseScroll = [this](Event* event) {
        auto mouseEvent = static_cast<EventMouse*>(event);
        float scrollY = mouseEvent->getScrollY() * 20; // ���������ٶ�
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
    // ����������
    auto selectedBg = DrawNode::create();
    selectedBg->drawSolidRect(
        Vec2(0, 0),
        Vec2(EDIT_SELECTED_WIDTH, EDIT_SELECTED_HEIGHT),
        Color4F(0.2f, 0.2f, 0.2f, 0.3f)  // ��͸����ɫ����
    );
    m_selectedContainer->addChild(selectedBg);
    this->addChild(m_selectedContainer);
}

void EditGroupScene::onCardSelected(int cardId) {
    // ����Ƿ��Ѿ�ѡ�������ſ�
    if (std::find(m_selectedCardIds.begin(), m_selectedCardIds.end(), cardId)
        != m_selectedCardIds.end()) {
        return;
    }
    m_selectedCardIds.push_back(cardId);
    // ���Ҳ����Сͼ��
    auto card = Sprite::create("cards/card.png");
    card->setScale(0.4f);
    // ����λ�ã�ÿ��5����
    int index = m_selectedCardIds.size() - 1;
    float cardSpacing = 100;
    float startX = 50;
    float startY = EDIT_SELECTED_HEIGHT - 100;
    float x = startX + (index % 5) * cardSpacing;
    float y = startY - (index / 5) * cardSpacing;
    card->setPosition(Vec2(x, y));
    card->setTag(cardId);
    // Ϊ�Ҳ࿨����ӵ��������
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
    // ����ѡ����Ч
    audioPlayer("Music/ClickSoundEffect.mp3", false);
}

void EditGroupScene::onCardRemoved(int cardId) {
    auto it = std::find(m_selectedCardIds.begin(), m_selectedCardIds.end(), cardId);
    if (it != m_selectedCardIds.end()) {
        // ����ѡ�������Ƴ�
        m_selectedCardIds.erase(it);
        // ��ȡ���п��ƾ��鲢��������
        std::vector<Sprite*> cards;
        for (auto child : m_selectedContainer->getChildren()) {
            // ���������ڵ㣨�����ڵ�� tag Ϊ0��
            if (child->getTag() > 0) {
                cards.push_back(static_cast<Sprite*>(child));
            }
        }
        // �����������п��Ƶ�λ��
        for (size_t i = 0; i < cards.size(); i++) {
            float x = 50 + (i % 5) * 100;  // ÿ��5�ţ����100
            float y = EDIT_SELECTED_HEIGHT - 100 - (i / 5) * 100;  // ������������

            // ʹ�ö����ƶ�����λ��
            cards[i]->runAction(MoveTo::create(0.2f, Vec2(x, y)));
        }
        // ����ȡ��ѡ����Ч
        audioPlayer("Music/ClickSoundEffect.mp3", false);
    }
}

void EditGroupScene::initButtons() {
    // ������ɰ�ť
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
    // ���ŵ����Ч
    audioPlayer("Music/ClickSoundEffect.mp3", false);

    // ���濨����Ϣ
    if (m_selectedCardIds.size() == 30) {
        // ʹ�� UserDefault ���濨����Ϣ
        auto userDefault = UserDefault::getInstance();
        for (int i = 0; i < 30; i++) {
            std::string key = "customDeck_" + std::to_string(i);
            userDefault->setIntegerForKey(key.c_str(), m_selectedCardIds[i]);
        }
        userDefault->flush();

        // �������˵�
        Director::getInstance()->popScene();
    }
    else {
        // ��ʾ��ʾ��Ϣ������������30�ſ���
        auto label = Label::createWithTTF("����������30�ſ��ƣ�", "fonts/arial.ttf", 24);
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
        // �������˵�
        Director::getInstance()->popScene();
    }
}