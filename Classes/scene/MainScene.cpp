// MainScene.cpp
#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "BoardScene.h"
#include "proj.win32/Alluse.h"
#include "network/CocosUIListener.h"

USING_NS_CC;

Scene* MainScene::createScene()
{
    return MainScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    // 设置背景音乐
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.5f); // 设置音量为 50%
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("../Resources/Music/MainScene.mp3", true);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. 创建菜单项

    // 创建 normalGame 按钮
    auto normalGame = MenuItemImage::create(
        "normalgame.png",
        "normalgame.png",
        CC_CALLBACK_1(MainScene::normalGameCallback, this));

    if (normalGame == nullptr ||
        normalGame->getContentSize().width <= 0 ||
        normalGame->getContentSize().height <= 0)
    {
        problemLoading("'normalgame.png' and 'normalgame.png'");
    }
    else
    {
        float x = visibleSize.width / 2;
        float y = origin.y + visibleSize.height / 2;
        normalGame->setPosition(Vec2(x + MAIN_SCENE_PLAYBUTTON_OFFSET_X, y + MAIN_SCENE_PLAYBUTTON_OFFSET_Y));
    }

    // 创建 adventureGame 按钮
    auto adventureGame = MenuItemImage::create(
        "adventure.png",
        "adventure.png",
        CC_CALLBACK_1(MainScene::adventureGameCallback, this));

    if (adventureGame == nullptr ||
        adventureGame->getContentSize().width <= 0 ||
        adventureGame->getContentSize().height <= 0)
    {
        problemLoading("'adventure.png' and 'adventure.png'");
    }
    else
    {
        float x = visibleSize.width / 2;
        float y = origin.y + visibleSize.height / 2;
        adventureGame->setPosition(Vec2(x + MAIN_SCENE_ADVBUTTON_OFFSET_X, y + MAIN_SCENE_ADVBUTTON_OFFSET_Y));
    }

    // 创建 collection 按钮
    auto collection = MenuItemImage::create(
        "collection.png",
        "collection.png",
        CC_CALLBACK_1(MainScene::collectionCallback, this));

    if (collection == nullptr ||
        collection->getContentSize().width <= 0 ||
        collection->getContentSize().height <= 0)
    {
        problemLoading("'collection.png' and 'collection.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - collection->getContentSize().width / 2;
        float y = origin.y + collection->getContentSize().height / 2;
        collection->setPosition(Vec2(x, y));
    }

    // 创建菜单
    auto menu = Menu::create(normalGame, adventureGame, collection, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. 添加背景精灵

    auto sprite = Sprite::create("../Resources/Scenes/2MainScene.png");
    if (sprite == nullptr)
    {
        problemLoading("'2MainScene.png'");
    }
    else
    {
        // 获取屏幕的大小
        auto visibleSize = Director::getInstance()->getVisibleSize();
        auto origin = Director::getInstance()->getVisibleOrigin();

        // 设置背景的位置，使其居中显示
        sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

        // 根据屏幕的大小调整背景的大小
        sprite->setScaleX(visibleSize.width / sprite->getContentSize().width);
        sprite->setScaleY(visibleSize.height / sprite->getContentSize().height);

        // 将背景添加到场景中
        this->addChild(sprite);
    }

    /////////////////////////////
    // 4. 创建并初始化 CocosUIListener 和 PhotonLib

    // 创建一个用于UI的Layer
    auto uiLayer = Layer::create();
    this->addChild(uiLayer);

    // 创建CocosUIListener并初始化日志标签
    cocosUIListener = new CocosUIListener();
    cocosUIListener->initializeLogLabel(uiLayer, Vec2(visibleSize.width / 2, visibleSize.height - 50));

    // 初始化PhotonLib
    photonLib = new PhotonLib(cocosUIListener);
    photonLib->setRoomJoinedCallback([=]() {
        // 切换到BoardScene
        Director::getInstance()->replaceScene(TransitionFade::create(0.2f, BoardScene::createScene()));
        });

    // 定期调用PhotonLib::update()
    this->schedule(CC_SCHEDULE_SELECTOR(MainScene::updatePhoton), 0.1f); // 每0.1秒调用一次

    return true;
}

void MainScene::updatePhoton(float dt)
{
    if (photonLib)
    {
        photonLib->update();
    }
}

void MainScene::normalGameCallback(cocos2d::Ref* pSender)
{
    // 调用PhotonLib的joinOrCreateRoom方法
    photonLib->joinOrCreateRoom(ExitGames::Common::JString(L"DefaultRoom")); // 可以自定义房间名
}

void MainScene::adventureGameCallback(cocos2d::Ref* pSender)
{
    // 实现冒险模式的逻辑
}

void MainScene::collectionCallback(cocos2d::Ref* pSender)
{
    // 实现收藏模式的逻辑
}
