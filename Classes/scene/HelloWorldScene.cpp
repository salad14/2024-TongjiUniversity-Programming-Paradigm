#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "MainScene.h"

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
    
    // ¼ÓÔØ±³¾°
    const auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    const auto background = Sprite::create("../Resources/Scenes/HelloWorld.png");
    if (background == nullptr) {
        problemLoading("../Resources/Scenes/HelloWorld.png");
        return false;
    }
    background->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2));
    this->addChild(background);
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}

void HelloWorld::menuReplaceCallback(Ref* pSender)
{
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f,MainScene::createScene()));
}