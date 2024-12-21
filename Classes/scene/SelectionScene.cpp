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

    
    for (int i = 0; i < 6; i++) {
        
        std::string filename = StringUtils::format("groups/group%d.png", i + 1);
        auto icon = Sprite::create(filename);
        icon->setScale(SELECT_ICON_SCALE);
        
        // 计算图标位置（3行2列排列）
        float x = SELECT_START_X + (i % SELECT_ICONS_PER_ROW) * SELECT_ICON_SPACING;
        float y = SELECT_START_Y - (i / SELECT_ICONS_PER_ROW) * SELECT_ICON_SPACING;
        icon->setPosition(Vec2(x, y));

        // 设置图标标签
        icon->setTag(i);

        // 初始化悬停状态
        m_isHovered[icon] = false;

        // 添加鼠标事件监听器
        auto listener = EventListenerMouse::create();

        listener->onMouseMove = [this, icon](EventMouse* event) {
            Vec2 locationInNode = icon->convertToNodeSpace(Vec2(event->getCursorX(), event->getCursorY()));
            Size s = icon->getContentSize();
            Rect rect = Rect(0, 0, s.width, s.height);
            bool isInside = rect.containsPoint(locationInNode);

            // 只在状态改变时才执行动画
            if (isInside != m_isHovered[icon]) {
                m_isHovered[icon] = isInside;

                // 停止当前正在进行的所有动作
                icon->stopAllActions();

                if (isInside) {
                    // 放大动画
                    icon->runAction(EaseElasticOut::create( ScaleTo::create(0.3f, SELECT_ICON_SCALE * 1.2f), 0.3f));
                }
                else {
                    // 缩小动画
                    icon->runAction(EaseBackIn::create( ScaleTo::create(0.2f, SELECT_ICON_SCALE)));
                }
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
    m_previewSprite->setPosition(Vec2(1600, 800));
    m_previewSprite->setScale(1.5f);
    this->addChild(m_previewSprite);

    // 创建卡组名称标签(加上联机功能好像采用了其他UI显示方法，目前基础的Label无法正常显示，类似问题亦可见于boardScene 12.21)
    m_groupNameLabel = Label::createWithTTF("卡组 1", "fonts/arial.ttf", 36);  // 增大字体大小
    m_groupNameLabel->setPosition(Vec2(1600, 10));   // 调整位置到预览图下方
    m_groupNameLabel->setColor(Color3B::WHITE);       // 设置文字颜色为白色
    m_groupNameLabel->enableOutline(Color4B::BLACK, 2);  // 添加黑色描边
    this->addChild(m_groupNameLabel, 1);  

    m_selectedIndex = 0;//默认选中第一卡组
    return true;
}

// 更新预览
void SelectionScene::updatePreview(int index)
{
    audioPlayer("Music/putcard.mp3", false);

    if (index < 0 || index >= 6) return;

    m_selectedIndex = index;

    auto scaleDown = EaseOut::create(
        ScaleTo::create(0.15f, 1.3f), // 从1.5缩小到1.3
        2.0f
    );
    auto fadeOut = EaseOut::create(
        FadeTo::create(0.15f, 200), // 降低透明度
        2.0f
    );
    // 更新图片的回调函数
    auto updateTexture = CallFunc::create([this, index]() {
        std::string filename = StringUtils::format("groups/group%d.png", index + 1);
        m_previewSprite->setTexture(filename);
        });
    auto scaleUp = EaseOut::create(
        ScaleTo::create(0.15f, 1.5f), // 恢复到1.5
        2.0f
    );
    auto fadeIn = EaseOut::create(
        FadeTo::create(0.15f, 255), // 恢复完全不透明
        2.0f
    );
    // 同时执行缩放和透明度动画
    auto scaleDownWithFade = Spawn::create(scaleDown, fadeOut, nullptr);
    auto scaleUpWithFade = Spawn::create(scaleUp, fadeIn, nullptr);
    // 按顺序执行所有动画
    auto sequence = Sequence::create(
        scaleDownWithFade,
        updateTexture,
        scaleUpWithFade,
        nullptr
    );
    m_previewSprite->runAction(sequence);
   
    //// 更新卡组名称（Label显示有误，该部分暂时删去）
    //std::string groupName = StringUtils::format("卡组 %d", index + 1);
    //m_groupNameLabel->setString(groupName);
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
  
    //在这里添加传递已选择卡组信息的方法
    // 
    // 通过全局变量传递？？？？
    // 目前打算在 Alluse.h 中定义一个全局变量，
    SELECT_CARDS = m_selectedIndex;//这是目前选中的卡组编号
    
    // 或者使用 Photon什么的传一个int？？？（待完善）

    //从此处开始游戏（12.20目前有误）
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MatchingScene::createScene()));
    
}