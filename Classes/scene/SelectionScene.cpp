/****************************************************************
 * 游戏前卡组选择界面的实现
 * Author:   Lee
 ****************************************************************/
#include "SelectionScene.h"
#include "MainScene.h"
#include "MatchingScene.h"
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"
#include "network/CocosUIListener.h"
#include "network/Photon_lib.h"

 // 命名空间
USING_NS_CC;

// 创建场景
Scene* SelectionScene::createScene()
{
    auto scene = Scene::create();
    auto layer = SelectionScene::create();
    scene->addChild(layer);
    return scene;
}

// 初始化场景
bool SelectionScene::init()
{
    // 创建场景
    if (!Scene::init()) {
        return false;
    }

    // 加载背景
    const auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    const auto background = Sprite::create("../Resources/Scenes/3SelectionScene.png");
    background->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2));
    this->addChild(background);


    // 创建返回按钮
    auto cancel = MenuItemImage::create(
        "button/cancel.png",
        "button/cancelSelected.png",
        CC_CALLBACK_1(SelectionScene::cancelCallback, this)
    );
    cancel->setPosition(Vec2(1900, 70)); 

    // 创建开始按钮
    auto start = MenuItemImage::create(
        "button/start.png",
        "button/startSelected.png",
        CC_CALLBACK_1(SelectionScene::startCallback, this)
    );
    start->setPosition(Vec2(1600, 240));
    auto menu = Menu::create(cancel, start, nullptr);
    // 添加菜单
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // 创建卡组图标
    float ICON_SCALE = 0.8f;  // 图标缩放比例
    float ICON_SPACING = 300.0;  // 图标间距
    float START_X = 300.0f;    // 第一个图标的X坐标
    float START_Y = 650.0f;    // 第一行图标的Y坐标
    int ICONS_PER_ROW = 3;     // 每行图标数

    
    for (int i = 0; i < 6; i++) {
        std::string filename = StringUtils::format("groups/group%d.png", i + 1);
        auto icon = Sprite::create(filename);
        icon->setScale(ICON_SCALE);

        // 计算图标位置（3行2列排列）
        float x = START_X + (i % ICONS_PER_ROW) * ICON_SPACING;
        float y = START_Y - (i / ICONS_PER_ROW) * ICON_SPACING;
        icon->setPosition(Vec2(x, y));

        // 设置图标标签
        icon->setTag(i);

        // 添加鼠标事件监听器
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

        // 鼠标点击事件
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

    // 创建右侧预览区域
    m_previewSprite = Sprite::create("groups/group1.png");
    m_previewSprite->setPosition(Vec2(1600, 500));
    m_previewSprite->setScale(1.5f);
    this->addChild(m_previewSprite);

    // 创建卡组名称标签
    m_groupNameLabel = Label::createWithTTF("卡组 1", "fonts/arial.ttf", 36);
    m_groupNameLabel->setPosition(Vec2(1600, 300));
    this->addChild(m_groupNameLabel);

    m_selectedIndex = 0;


    return true;
}

// 更新预览
void SelectionScene::updatePreview(int index)
{
    if (index < 0 || index >= 6) return;

    m_selectedIndex = index;

    // 更新预览图
    std::string filename = StringUtils::format("groups/group%d.png", index + 1);
    m_previewSprite->setTexture(filename);

    // 更新卡组名称
    std::string groupName = StringUtils::format("卡组 %d", index + 1);
    m_groupNameLabel->setString(groupName);
}

//返回按钮
void SelectionScene::cancelCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("Music/ClickSoundEffect.mp3", false);
   
    // 使用 popScene 从场景栈弹出设置场景，返回到之前保存的主菜单场景
    Director::getInstance()->popScene();
    
    //Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene(), Color3B::WHITE));
    //Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene()));
    
}

//开始游戏按钮
void SelectionScene::startCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("Music/ClickSoundEffect.mp3", false);
   
    //从此处开始游戏（12.20目前有误）
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MatchingScene::createScene()));
    
}