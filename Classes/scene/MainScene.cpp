#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "scene/BoardScene.h"

USING_NS_CC;

Scene* MainScene::createScene()
{
    return MainScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
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

    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.5f); // ��������Ϊ 50%
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("../Resources/Music/MainScene.mp3", true);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object

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
        normalGame->setPosition(Vec2(x, y));
    }


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
        float y = origin.y + adventureGame->getContentSize().height / 2;
        adventureGame->setPosition(Vec2(x, y));
    }

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



    // create menu, it's an autorelease object
    auto menu = Menu::create(normalGame, adventureGame, collection, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    // add "HelloWorld" splash screen"

    auto sprite = Sprite::create("mainbackground.png");
    if (sprite == nullptr)
    {
        problemLoading("'mainbackground.png'");
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
        this->addChild(sprite);;
    }
    return true;
}


void MainScene::normalGameCallback(Ref* pSender)
{
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, BoardScene::createScene()));
}

void MainScene::adventureGameCallback(Ref* pSender)
{

}

void MainScene::collectionCallback(Ref* pSender)
{

}