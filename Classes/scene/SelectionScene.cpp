/****************************************************************
 * ��Ϸǰ����ѡ������ʵ��
 * Author:   Lee
 ****************************************************************/
#include "SelectionScene.h"
#include "MainScene.h"
#include "MatchingScene.h"
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"
#include "network/CocosUIListener.h"
#include "network/Photon_lib.h"

 // �����ռ�
USING_NS_CC;

// ��������
Scene* SelectionScene::createScene()
{
    auto scene = Scene::create();
    auto layer = SelectionScene::create();
    scene->addChild(layer);
    return scene;
}

// ��ʼ������
bool SelectionScene::init()
{
    // ��������
    if (!Scene::init()) {
        return false;
    }

    // ���ر���
    const auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    const auto background = Sprite::create("../Resources/Scenes/3SelectionScene.png");
    background->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2));
    this->addChild(background);


    // �������ذ�ť
    auto cancel = MenuItemImage::create(
        "button/cancel.png",
        "button/cancelSelected.png",
        CC_CALLBACK_1(SelectionScene::cancelCallback, this)
    );
    cancel->setPosition(Vec2(1900, 70)); 

    // ������ʼ��ť
    auto start = MenuItemImage::create(
        "button/start.png",
        "button/startSelected.png",
        CC_CALLBACK_1(SelectionScene::startCallback, this)
    );
    start->setPosition(Vec2(1600, 240));
    auto menu = Menu::create(cancel, start, nullptr);
    // ��Ӳ˵�
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // ��������ͼ��
    float ICON_SCALE = 0.8f;  // ͼ�����ű���
    float ICON_SPACING = 300.0;  // ͼ����
    float START_X = 300.0f;    // ��һ��ͼ���X����
    float START_Y = 650.0f;    // ��һ��ͼ���Y����
    int ICONS_PER_ROW = 3;     // ÿ��ͼ����

    
    for (int i = 0; i < 6; i++) {
        std::string filename = StringUtils::format("groups/group%d.png", i + 1);
        auto icon = Sprite::create(filename);
        icon->setScale(ICON_SCALE);

        // ����ͼ��λ�ã�3��2�����У�
        float x = START_X + (i % ICONS_PER_ROW) * ICON_SPACING;
        float y = START_Y - (i / ICONS_PER_ROW) * ICON_SPACING;
        icon->setPosition(Vec2(x, y));

        // ����ͼ���ǩ
        icon->setTag(i);

        // �������¼�������
        auto listener = EventListenerMouse::create();

        // 
        listener->onMouseMove = [ICON_SCALE, this, icon](EventMouse* event) {
            Vec2 locationInNode = icon->convertToNodeSpace(Vec2(event->getCursorX(), event->getCursorY()));
            Size s = icon->getContentSize();
            Rect rect = Rect(0, 0, s.width, s.height);

            if (rect.containsPoint(locationInNode)) {
                icon->runAction(ScaleTo::create(0.1f, ICON_SCALE * 1.1f));
            }
            else {
                icon->runAction(ScaleTo::create(0.1f, ICON_SCALE));
            }
        };

        // ������¼�
        auto touchListener = EventListenerTouchOneByOne::create();
        touchListener->onTouchBegan = [this, icon](Touch* touch, Event* event) {
            Vec2 locationInNode = icon->convertToNodeSpace(touch->getLocation());
            Size s = icon->getContentSize();
            Rect rect = Rect(0, 0, s.width, s.height);

            if (rect.containsPoint(locationInNode)) {
                updatePreview(icon->getTag());
                return true;
            }
            return false;
        };

        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, icon);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, icon);

        this->addChild(icon);
        m_groupIcons.pushBack(icon);
    }

    // �����Ҳ�Ԥ������
    m_previewSprite = Sprite::create("groups/group1.png");
    m_previewSprite->setPosition(Vec2(1600, 500));
    m_previewSprite->setScale(1.5f);
    this->addChild(m_previewSprite);

    // �����������Ʊ�ǩ
    m_groupNameLabel = Label::createWithTTF("���� 1", "fonts/arial.ttf", 36);
    m_groupNameLabel->setPosition(Vec2(1600, 300));
    this->addChild(m_groupNameLabel);

    m_selectedIndex = 0;


    return true;
}

// ����Ԥ��
void SelectionScene::updatePreview(int index)
{
    if (index < 0 || index >= 6) return;

    m_selectedIndex = index;

    // ����Ԥ��ͼ
    std::string filename = StringUtils::format("groups/group%d.png", index + 1);
    m_previewSprite->setTexture(filename);

    // ���¿�������
    std::string groupName = StringUtils::format("���� %d", index + 1);
    m_groupNameLabel->setString(groupName);
}

//���ذ�ť
void SelectionScene::cancelCallback(Ref* pSender)
{
    // ���ص����Ч
    audioPlayer("Music/ClickSoundEffect.mp3", false);
   
    // ʹ�� popScene �ӳ���ջ�������ó��������ص�֮ǰ��������˵�����
    Director::getInstance()->popScene();
    
    //Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene(), Color3B::WHITE));
    //Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene()));
    
}

//��ʼ��Ϸ��ť
void SelectionScene::startCallback(Ref* pSender)
{
    // ���ص����Ч
    audioPlayer("Music/ClickSoundEffect.mp3", false);
   
    //�Ӵ˴���ʼ��Ϸ��12.20Ŀǰ����
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MatchingScene::createScene()));
    
}