/****************************************************************
 * 加载完成开始界面实现
 * Author:  Lee
 * 注：利用了系统初始化时遗留的HelloWorld场景文件，请注意名称区分
 ****************************************************************/
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "MainScene.h"
#include "proj.win32/AudioPlayer.h"
#include "network/CocosUIListener.h"
#include "network/Photon_lib.h"

USING_NS_CC;

// 创建场景
Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// 打印错误信息
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// 初始化
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建关闭按钮
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width / 2;
        float y = origin.y + closeItem->getContentSize().height / 2;
        closeItem->setPosition(Vec2(x, y));
    }

    // 创建开始游戏按钮
    auto startGame = MenuItemImage::create(
        "GameStart.png",
        "GameStartSelected.png",
        CC_CALLBACK_1(HelloWorld::menuReplaceCallback, this));
    if (startGame == nullptr ||
        startGame->getContentSize().width <= 0 ||
        startGame->getContentSize().height <= 0)
    {
        problemLoading("'GameStart.png' and 'GameStartSelected.png'");
    }
    else
    {
        float x = visibleSize.width / 2;
        float y = origin.y + startGame->getContentSize().height / 2;
        startGame->setPosition(Vec2(x, y));
    }

    // 创建菜单
    auto menu = Menu::create(closeItem, startGame, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // 加载背景
    const auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    const auto background = Sprite::create("../Resources/Scenes/1HelloWorld.png");
    if (background == nullptr) {
        problemLoading("../Resources/Scenes/1HelloWorld.png");
        return false;
    }
    background->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2));
    this->addChild(background);

    /////////////////////////////
    // 2. 初始化 CocosUIListener

    // 获取 Singleton 的 CocosUIListener 实例
    CocosUIListener* cocosUIListener = CocosUIListener::getInstance();

    // 创建一个用于UI的Layer
    auto uiLayer = Layer::create();
    this->addChild(uiLayer);

    // 将 logLabel 附加到当前 Layer 上
    cocosUIListener->attachToLayer(uiLayer, Vec2(visibleSize.width / 2, visibleSize.height - 50));

    return true;
}

// 关闭按钮回调
void HelloWorld::menuCloseCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
    // 关闭游戏
    Director::getInstance()->end();

    /* To navigate back to native iOS screen(if present) without quitting the application, do not use Director::getInstance()->end() as given above,
       instead trigger a custom event created in RootViewController.mm as below */

       // EventCustom customEndEvent("game_scene_close_event");
       // _eventDispatcher->dispatchEvent(&customEndEvent);
}

// 开始游戏按钮回调
void HelloWorld::menuReplaceCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);

    // 获取 Singleton 的 CocosUIListener 实例
    CocosUIListener* cocosUIListener = CocosUIListener::getInstance();

    // 初始化 PhotonLib 并传递 CocosUIListener 作为监听器
    PhotonLib::initialize(cocosUIListener);
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (!photonLib)
    {
        CCLOG("PhotonLib initialization failed!");
        cocosUIListener->writeString(L"PhotonLib initialization failed!");
        return;
    }

    // 设置连接回调，连接成功后切换到 MainScene
    photonLib->setConnectionCallback([=](bool success, const std::wstring& message) {
        if (success)
        {
            CCLOG("Connected to Photon successfully. Switching to MainScene.");
            cocosUIListener->writeString(L"Connected to Photon successfully. Switching to MainScene.");
            // 切换到 MainScene
            Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene()));
        }
        else
        {
            CCLOG("Failed to connect to Photon: %ls", message.c_str());
            // 在UI上显示错误信息
            cocosUIListener->writeString(ExitGames::Common::JString(L"Failed to connect to Photon: ") + ExitGames::Common::JString(message.c_str()));
            // 可在此添加重试逻辑或其他错误处理
        }
        });

    // 开始连接到 Photon 服务器
    CCLOG("Connecting to Photon from HelloWorldScene...");
    cocosUIListener->writeString(L"Connecting to Photon...");
    photonLib->connectToPhoton(); // 开始连接

    // 定期调用 PhotonLib::update()
    this->schedule([=](float dt) {
        PhotonLib::getInstance()->update();
        }, "PhotonUpdate");
}
