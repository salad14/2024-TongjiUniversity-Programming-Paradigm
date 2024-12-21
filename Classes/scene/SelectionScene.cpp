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

    
    for (int i = 0; i < 6; i++) {
        
        std::string filename = StringUtils::format("groups/group%d.png", i + 1);
        auto icon = Sprite::create(filename);
        icon->setScale(SELECT_ICON_SCALE);
        
        // ����ͼ��λ�ã�3��2�����У�
        float x = SELECT_START_X + (i % SELECT_ICONS_PER_ROW) * SELECT_ICON_SPACING;
        float y = SELECT_START_Y - (i / SELECT_ICONS_PER_ROW) * SELECT_ICON_SPACING;
        icon->setPosition(Vec2(x, y));

        // ����ͼ���ǩ
        icon->setTag(i);

        // ��ʼ����ͣ״̬
        m_isHovered[icon] = false;

        // �������¼�������
        auto listener = EventListenerMouse::create();

        listener->onMouseMove = [this, icon](EventMouse* event) {
            Vec2 locationInNode = icon->convertToNodeSpace(Vec2(event->getCursorX(), event->getCursorY()));
            Size s = icon->getContentSize();
            Rect rect = Rect(0, 0, s.width, s.height);
            bool isInside = rect.containsPoint(locationInNode);

            // ֻ��״̬�ı�ʱ��ִ�ж���
            if (isInside != m_isHovered[icon]) {
                m_isHovered[icon] = isInside;

                // ֹͣ��ǰ���ڽ��е����ж���
                icon->stopAllActions();

                if (isInside) {
                    // �Ŵ󶯻�
                    icon->runAction(EaseElasticOut::create( ScaleTo::create(0.3f, SELECT_ICON_SCALE * 1.2f), 0.3f));
                }
                else {
                    // ��С����
                    icon->runAction(EaseBackIn::create( ScaleTo::create(0.2f, SELECT_ICON_SCALE)));
                }
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
    m_previewSprite->setPosition(Vec2(1600, 800));
    m_previewSprite->setScale(1.5f);
    this->addChild(m_previewSprite);

    // �����������Ʊ�ǩ(�����������ܺ������������UI��ʾ������Ŀǰ������Label�޷�������ʾ������������ɼ���boardScene 12.21)
    m_groupNameLabel = Label::createWithTTF("���� 1", "fonts/arial.ttf", 36);  // ���������С
    m_groupNameLabel->setPosition(Vec2(1600, 10));   // ����λ�õ�Ԥ��ͼ�·�
    m_groupNameLabel->setColor(Color3B::WHITE);       // ����������ɫΪ��ɫ
    m_groupNameLabel->enableOutline(Color4B::BLACK, 2);  // ��Ӻ�ɫ���
    this->addChild(m_groupNameLabel, 1);  

    m_selectedIndex = 0;//Ĭ��ѡ�е�һ����
    return true;
}

// ����Ԥ��
void SelectionScene::updatePreview(int index)
{
    audioPlayer("Music/putcard.mp3", false);

    if (index < 0 || index >= 6) return;

    m_selectedIndex = index;

    auto scaleDown = EaseOut::create(
        ScaleTo::create(0.15f, 1.3f), // ��1.5��С��1.3
        2.0f
    );
    auto fadeOut = EaseOut::create(
        FadeTo::create(0.15f, 200), // ����͸����
        2.0f
    );
    // ����ͼƬ�Ļص�����
    auto updateTexture = CallFunc::create([this, index]() {
        std::string filename = StringUtils::format("groups/group%d.png", index + 1);
        m_previewSprite->setTexture(filename);
        });
    auto scaleUp = EaseOut::create(
        ScaleTo::create(0.15f, 1.5f), // �ָ���1.5
        2.0f
    );
    auto fadeIn = EaseOut::create(
        FadeTo::create(0.15f, 255), // �ָ���ȫ��͸��
        2.0f
    );
    // ͬʱִ�����ź�͸���ȶ���
    auto scaleDownWithFade = Spawn::create(scaleDown, fadeOut, nullptr);
    auto scaleUpWithFade = Spawn::create(scaleUp, fadeIn, nullptr);
    // ��˳��ִ�����ж���
    auto sequence = Sequence::create(
        scaleDownWithFade,
        updateTexture,
        scaleUpWithFade,
        nullptr
    );
    m_previewSprite->runAction(sequence);
   
    //// ���¿������ƣ�Label��ʾ���󣬸ò�����ʱɾȥ��
    //std::string groupName = StringUtils::format("���� %d", index + 1);
    //m_groupNameLabel->setString(groupName);
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
  
    //��������Ӵ�����ѡ������Ϣ�ķ���
    // 
    // ͨ��ȫ�ֱ������ݣ�������
    // Ŀǰ������ Alluse.h �ж���һ��ȫ�ֱ�����
    SELECT_CARDS = m_selectedIndex;//����Ŀǰѡ�еĿ�����
    
    // ����ʹ�� Photonʲô�Ĵ�һ��int�������������ƣ�

    //�Ӵ˴���ʼ��Ϸ��12.20Ŀǰ����
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MatchingScene::createScene()));
    
}