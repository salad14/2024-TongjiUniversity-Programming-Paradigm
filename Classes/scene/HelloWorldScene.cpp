/****************************************************************
 * 加载完成开始界面实现
 * Author:  Lee
 * 注：利用了系统初始化时遗留的Helloworld场景文件，请注意名称区分
 ****************************************************************/
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "MainScene.h"
#include "proj.win32/AudioPlayer.h"
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

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
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

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


    // create menu, it's an autorelease object
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

    return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}


//最后应修改的版本
/*
void HelloWorld::menuReplaceCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);

    // 创建一个用于UI的Layer
    auto uiLayer = Layer::create();
    this->addChild(uiLayer);

    // 创建CocosUIListener并初始化日志标签
    cocosUIListener = new CocosUIListener();
    cocosUIListener->initializeLogLabel(uiLayer, Vec2(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height - 50));

    // 初始化PhotonLib单例
    PhotonLib::initialize(cocosUIListener);
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (!photonLib)
    {
        CCLOG("PhotonLib initialization failed!");
        return;
    }

    // 设置连接成功后的回调
    photonLib->setConnectionCallback([=](bool success, const std::wstring& message) {
        if (success)
        {
            CCLOG("Connected to Photon successfully. Switching to MainScene.");
            // 切换到 MainScene
            Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene()));
        }
        else
        {
            CCLOG("Failed to connect to Photon: %ls", message.c_str());
            // 可以在这里添加重试逻辑或显示错误信息
        }
        });

    // 开始连接到Photon服务器
    CCLOG("Connecting to Photon from HelloWorldScene...");
    photonLib->connectToPhoton(); // 开始连接

    // 注意：不立即切换场景，等待连接回调触发
}
*/


//测试用
void HelloWorld::menuReplaceCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);

    // 创建一个用于UI的Layer
    auto uiLayer = Layer::create();
    this->addChild(uiLayer);

    // 创建CocosUIListener并初始化日志标签
    cocosUIListener = new CocosUIListener();
    cocosUIListener->initializeLogLabel(uiLayer, Vec2(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height - 50));

    // 初始化PhotonLib单例
    /*
    PhotonLib::initialize(cocosUIListener);
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (!photonLib)
    {
        CCLOG("PhotonLib initialization failed!");
        // 即使初始化失败，也切换到 MainScene
    }
    else
    {
        // 设置连接成功或失败的回调，仅用于日志记录，不负责场景切换
        photonLib->setConnectionCallback([=](bool success, const std::wstring& message) {
            if (success)
            {
                CCLOG("Connected to Photon successfully.");
            }
            else
            {
                CCLOG("Failed to connect to Photon: %ls", message.c_str());
            }
            // 场景切换不在这里进行
            });

        // 开始连接到Photon服务器
        CCLOG("Connecting to Photon from HelloWorldScene...");
        photonLib->connectToPhoton(); // 开始连接
    }*/

    // 无论连接是否成功，立即切换到 MainScene
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene()));
}
