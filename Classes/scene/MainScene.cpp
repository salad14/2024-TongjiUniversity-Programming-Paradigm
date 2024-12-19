/****************************************************************
* ���˵�ҳ���ʵ��
* Author: Lee
 ****************************************************************/

#include "MainScene.h"
#include "SetScene.h"
#include "SimpleAudioEngine.h"
#include "BoardScene.h"
#include "MatchingScene.h"
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"
#include "network/CocosUIListener.h"
#include "network/Photon_lib.h"

USING_NS_CC;

Scene* MainScene::createScene()
{
    return MainScene::create();
}

// ���캯��
MainScene::MainScene()
{
}

// ��������
MainScene::~MainScene()
{
}

// ��ӡ���ش�����Ϣ
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainScene.cpp\n");
}

// ��ʼ������
bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. �����˵���

    // normalgame��ť����ת����ͨģʽ���棩
    auto normalGame = MenuItemImage::create(
        "../Resources/button/normalgame.png",
        "../Resources/button/normalgame.png",
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

    // adventure��ť����ת��ð��ģʽ���棩
    auto adventureGame = MenuItemImage::create(
        "../Resources/button/adventure.png",
        "../Resources/button/adventure.png",
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

    // collection��ť����ת���Զ��忨�ƽ��棩
    auto collection = MenuItemImage::create(
        "../Resources/button/collection.png",
        "../Resources/button/collection.png",
        CC_CALLBACK_1(MainScene::collectionCallback, this));

    if (collection == nullptr ||
        collection->getContentSize().width <= 0 ||
        collection->getContentSize().height <= 0)
    {
        problemLoading("'collection.png' and 'collection.png'");
    }
    else
    {
        float x = origin.x + getContentSize().width / 2;
        float y = origin.y + 100;
        collection->setPosition(Vec2(x, y));
    }

    // set��ť����ת�����ý��棩
    auto set = MenuItemImage::create(
        "../Resources/button/set.png",
        "../Resources/button/setSelected.png",
        CC_CALLBACK_1(MainScene::setCallback, this));
    if (set == nullptr ||
        set->getContentSize().width <= 0 ||
        set->getContentSize().height <= 0)
    {
        problemLoading("'set.png' and 'collection.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - set->getContentSize().width / 2;
        float y = origin.y + set->getContentSize().height / 2;
        set->setPosition(Vec2(x, y));
    }

    // �����˵�
    auto menu = Menu::create(normalGame, adventureGame, collection, set, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. ���ӱ�������

    auto sprite = Sprite::create("../Resources/Scenes/2MainScene.png");
    if (sprite == nullptr)
    {
        problemLoading("'2MainScene.png'");
    }
    else
    {
        // ��ȡ��Ļ�Ĵ�С
        auto visibleSize = Director::getInstance()->getVisibleSize();
        auto origin = Director::getInstance()->getVisibleOrigin();

        // ���ñ�����λ�ã�ʹ�������ʾ
        sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

        // ������Ļ�Ĵ�С���������Ĵ�С
        sprite->setScaleX(visibleSize.width / sprite->getContentSize().width);
        sprite->setScaleY(visibleSize.height / sprite->getContentSize().height);

        // ���������ӵ�������
        this->addChild(sprite);
    }

    /////////////////////////////
    // 4. ��ʼ�� CocosUIListener

    // ��ȡ Singleton �� CocosUIListener ʵ��
    CocosUIListener* cocosUIListener = CocosUIListener::getInstance();

    // ����һ������UI��Layer
    auto uiLayer = Layer::create();
    this->addChild(uiLayer);

    // �� logLabel ���ӵ���ǰ Layer ��
    cocosUIListener->attachToLayer(uiLayer, Vec2(visibleSize.width / 2, visibleSize.height - 50));
    return true;
}

//�ú�������û��ʹ�ã�
// ���ڸ���Photon
//void MainScene::updatePhoton(float dt)
//{
//    PhotonLib* photonLib = PhotonLib::getInstance();
//    if (photonLib)
//    {
//        photonLib->update();
//    }
//}

// normalGame ��ť�Ļص�
void MainScene::normalGameCallback(Ref* pSender)
{
    // ���ص����Ч
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MatchingScene::createScene()));
}

// adventureGame ��ť�Ļص�
void MainScene::adventureGameCallback(Ref* pSender)
{
    // ���ص����Ч
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
}

// collection ��ť�Ļص�
void MainScene::collectionCallback(Ref* pSender)
{
    // ���ص����Ч
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
}

// set ��ť�Ļص�
void MainScene::setCallback(Ref* pSender)
{
    // ���ص����Ч
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);

    // ʹ�� pushScene���������˵������ڳ���ջ��
    Director::getInstance()->pushScene(SetScene::create());
   
}
