/****************************************************************
* 主菜单页面的实现
* Author: Lee
 ****************************************************************/

#include "MainScene.h"
#include "SetScene.h"
#include "EditGroupScene.h"
#include "SimpleAudioEngine.h"
#include "BoardScene.h"
#include "MatchingScene.h"
#include "SelectionScene.h"
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"
#include "network/CocosUIListener.h"
#include "network/Photon_lib.h"

USING_NS_CC;

Scene* MainScene::createScene()
{
    return MainScene::create();
}

// 构造函数
MainScene::MainScene()
{
}

// 析构函数
MainScene::~MainScene()
{
}

// 打印加载错误信息
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainScene.cpp\n");
}

// 初始化场景
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
    // 2. 创建菜单项

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
        normalGame->setPosition(Vec2(x + MAIN_SCENE_PLAYBUTTON_OFFSET_X, y + MAIN_SCENE_PLAYBUTTON_OFFSET_Y));
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
        adventureGame->setPosition(Vec2(x + MAIN_SCENE_ADVBUTTON_OFFSET_X, y + MAIN_SCENE_ADVBUTTON_OFFSET_Y));
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
        float x = origin.x + getContentSize().width / 2;
        float y = origin.y + 100;
        collection->setPosition(Vec2(x, y));
    }

    // set按钮（跳转到设置界面）
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

    // 创建菜单
    auto menu = Menu::create(normalGame, adventureGame, collection, set, nullptr);
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
    // 4. 初始化 CocosUIListener

    // 获取 Singleton 的 CocosUIListener 实例
    CocosUIListener* cocosUIListener = CocosUIListener::getInstance();

    // 创建一个用于UI的Layer
    auto uiLayer = Layer::create();
    this->addChild(uiLayer);

    // 将 logLabel 附加到当前 Layer 上
    cocosUIListener->attachToLayer(uiLayer, Vec2(visibleSize.width / 2, visibleSize.height - 50));
    return true;
}

//该函数好像没有使用？
 //定期更新Photon
void MainScene::updatePhoton(float dt)
{
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (photonLib)
    {
        photonLib->update();
    }
}

// normalGame 按钮的回调
void MainScene::normalGameCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
    
    // 使用 pushScene，保留主菜单场景在场景栈中
    Director::getInstance()->pushScene(SelectionScene::create());
    //Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MatchingScene::createScene()));
    //Director::getInstance()->replaceScene(TransitionFade::create(0.2f, SelectionScene::createScene()));

}

// adventureGame 按钮的回调
void MainScene::adventureGameCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
}

// collection 按钮的回调
void MainScene::collectionCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
    // 使用 pushScene，保留主菜单场景在场景栈中
    //Director::getInstance()->pushScene(SelectionScene::create());
    //Director::getInstance()->pushScene(EditGroupScene::create());
}

// set 按钮的回调
void MainScene::setCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);

    // 使用 pushScene，保留主菜单场景在场景栈中
    Director::getInstance()->pushScene(SetScene::create());
   
}
