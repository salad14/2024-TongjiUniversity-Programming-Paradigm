/****************************************************************
 * ������ɿ�ʼ����ʵ��
 * Author:  Lee
 * ע��������ϵͳ��ʼ��ʱ������HelloWorld�����ļ�����ע����������
 ****************************************************************/
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "MainScene.h"
#include "proj.win32/AudioPlayer.h"
#include "network/CocosUIListener.h"
#include "network/Photon_lib.h"

USING_NS_CC;

// ��������
Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// ��ӡ������Ϣ
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// ��ʼ��
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

    // �����رհ�ť
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

    // ������ʼ��Ϸ��ť
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

    // �����˵�
    auto menu = Menu::create(closeItem, startGame, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // ���ر���
    const auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    const auto background = Sprite::create("../Resources/Scenes/1HelloWorld.png");
    if (background == nullptr) {
        problemLoading("../Resources/Scenes/1HelloWorld.png");
        return false;
    }
    background->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2));
    this->addChild(background);

    /////////////////////////////
    // 2. ��ʼ�� CocosUIListener

    // ��ȡ Singleton �� CocosUIListener ʵ��
    CocosUIListener* cocosUIListener = CocosUIListener::getInstance();

    // ����һ������UI��Layer
    auto uiLayer = Layer::create();
    this->addChild(uiLayer);

    // �� logLabel ���ӵ���ǰ Layer ��
    cocosUIListener->attachToLayer(uiLayer, Vec2(visibleSize.width / 2, visibleSize.height - 50));

    return true;
}

// �رհ�ť�ص�
void HelloWorld::menuCloseCallback(Ref* pSender)
{
    // ���ص����Ч
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
    // �ر���Ϸ
    Director::getInstance()->end();

    /* To navigate back to native iOS screen(if present) without quitting the application, do not use Director::getInstance()->end() as given above,
       instead trigger a custom event created in RootViewController.mm as below */

       // EventCustom customEndEvent("game_scene_close_event");
       // _eventDispatcher->dispatchEvent(&customEndEvent);
}

// ��ʼ��Ϸ��ť�ص�
void HelloWorld::menuReplaceCallback(Ref* pSender)
{
    // ���ص����Ч
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);

    // ��ȡ Singleton �� CocosUIListener ʵ��
    CocosUIListener* cocosUIListener = CocosUIListener::getInstance();

    // ��ʼ�� PhotonLib ������ CocosUIListener ��Ϊ������
    PhotonLib::initialize(cocosUIListener);
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (!photonLib)
    {
        CCLOG("PhotonLib initialization failed!");
        cocosUIListener->writeString(L"PhotonLib initialization failed!");
        return;
    }

    // �������ӻص������ӳɹ����л��� MainScene
    photonLib->setConnectionCallback([=](bool success, const std::wstring& message) {
        if (success)
        {
            CCLOG("Connected to Photon successfully. Switching to MainScene.");
            cocosUIListener->writeString(L"Connected to Photon successfully. Switching to MainScene.");
            // �л��� MainScene
            Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene()));
        }
        else
        {
            CCLOG("Failed to connect to Photon: %ls", message.c_str());
            // ��UI����ʾ������Ϣ
            cocosUIListener->writeString(ExitGames::Common::JString(L"Failed to connect to Photon: ") + ExitGames::Common::JString(message.c_str()));
            // ���ڴ���������߼�������������
        }
        });

    // ��ʼ���ӵ� Photon ������
    CCLOG("Connecting to Photon from HelloWorldScene...");
    cocosUIListener->writeString(L"Connecting to Photon...");
    photonLib->connectToPhoton(); // ��ʼ����

    // ���ڵ��� PhotonLib::update()
    this->schedule([=](float dt) {
        PhotonLib::getInstance()->update();
        }, "PhotonUpdate");
}
