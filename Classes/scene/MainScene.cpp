/****************************************************************
* 主菜单页面的实现
* Author: Lee
 ****************************************************************/

#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "MatchingScene.h"
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"
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

    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.5f); // 设置音量为 50%
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("../Resources/Music/MainScene.mp3", true);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // normalgame按钮（跳转到普通模式界面）
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
        normalGame->setPosition(Vec2(x+ MAIN_SCENE_PLAYBUTTON_OFFSET_X, y+ MAIN_SCENE_PLAYBUTTON_OFFSET_Y));
    }

    // adventure按钮（跳转到冒险模式界面）
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
        adventureGame->setPosition(Vec2(x+ MAIN_SCENE_ADVBUTTON_OFFSET_X, y+ MAIN_SCENE_ADVBUTTON_OFFSET_Y));
    }

    // collection按钮（跳转到自定义卡牌界面）
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
        float x = origin.x + visibleSize.width - collection->getContentSize().width / 2;
        float y = origin.y + collection->getContentSize().height / 2;
        collection->setPosition(Vec2(x, y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(normalGame, adventureGame, collection, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // 菜单界面背景
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
        this->addChild(sprite);;
    }
    return true;
}


void MainScene::normalGameCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MatchingScene::createScene()));
}

void MainScene::adventureGameCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
}

void MainScene::collectionCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
}