#include "MainScene.h"
#include "SetScene.h"
#include "BoardSceneOffline.h"
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

// ï¿½ï¿½ï¿½ìº¯ï¿½ï¿½
MainScene::MainScene()
{
}

// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
MainScene::~MainScene()
{
}

// ï¿½ï¿½Ó¡ï¿½ï¿½ï¿½Ø´ï¿½ï¿½ï¿½ï¿½ï¿½Ï¢
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainScene.cpp\n");
}

// ï¿½ï¿½Ê¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
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
    // 2. ï¿½ï¿½ï¿½ï¿½ï¿½Ëµï¿½ï¿½ï¿½

    // normalgameï¿½ï¿½Å¥ï¿½ï¿½ï¿½ï¿½×ªï¿½ï¿½ï¿½ï¿½Í¨Ä£Ê½ï¿½ï¿½ï¿½æ£©
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

    // adventureï¿½ï¿½Å¥ï¿½ï¿½ï¿½ï¿½×ªï¿½ï¿½Ã°ï¿½ï¿½Ä£Ê½ï¿½ï¿½ï¿½æ£©
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

    // collectionï¿½ï¿½Å¥ï¿½ï¿½ï¿½ï¿½×ªï¿½ï¿½ï¿½Ô¶ï¿½ï¿½å¿¨ï¿½Æ½ï¿½ï¿½æ£©
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

    // setï¿½ï¿½Å¥ï¿½ï¿½ï¿½ï¿½×ªï¿½ï¿½ï¿½ï¿½ï¿½Ã½ï¿½ï¿½æ£©
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

    // ï¿½ï¿½ï¿½ï¿½ï¿½Ëµï¿½
    auto menu = Menu::create(normalGame, adventureGame, collection, set, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. ï¿½ï¿½ï¿½Ó±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½

    auto sprite = Sprite::create("../Resources/Scenes/2MainScene.png");
    if (sprite == nullptr)
    {
        problemLoading("'2MainScene.png'");
    }
    else
    {
        // ï¿½ï¿½È¡ï¿½ï¿½Ä»ï¿½Ä´ï¿½Ð¡
        auto visibleSize = Director::getInstance()->getVisibleSize();
        auto origin = Director::getInstance()->getVisibleOrigin();

        // ï¿½ï¿½ï¿½Ã±ï¿½ï¿½ï¿½ï¿½ï¿½Î»ï¿½Ã£ï¿½Ê¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½?
        sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

        // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ä»ï¿½Ä´ï¿½Ð¡ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ä´ï¿½Ð¡
        sprite->setScaleX(visibleSize.width / sprite->getContentSize().width);
        sprite->setScaleY(visibleSize.height / sprite->getContentSize().height);

        // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Óµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
        this->addChild(sprite);
    }

    /////////////////////////////
    // 4. ï¿½ï¿½Ê¼ï¿½ï¿½ CocosUIListener

    // ï¿½ï¿½È¡ Singleton ï¿½ï¿½ CocosUIListener Êµï¿½ï¿½
    CocosUIListener* cocosUIListener = CocosUIListener::getInstance();

    // ï¿½ï¿½ï¿½ï¿½Ò»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½UIï¿½ï¿½Layer
    auto uiLayer = Layer::create();
    this->addChild(uiLayer);

    // ï¿½ï¿½ logLabel ï¿½ï¿½ï¿½Óµï¿½ï¿½ï¿½Ç° Layer ï¿½ï¿½
    cocosUIListener->attachToLayer(uiLayer, Vec2(visibleSize.width / 2, visibleSize.height - 50));
    return true;
}

//ï¿½Ãºï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ã»ï¿½ï¿½Ê¹ï¿½Ã£ï¿½
 //ï¿½ï¿½ï¿½Ú¸ï¿½ï¿½ï¿½Photon
void MainScene::updatePhoton(float dt)
{
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (photonLib)
    {
        photonLib->update();
    }
}

// normalGame ÆÕÍ¨Áª»úÄ£Ê½°´Å¥»Øµ÷
void MainScene::normalGameCallback(Ref* pSender)
{
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
    
    // Ê¹ÓÃ pushScene£¬±£ÁôÖ÷²Ëµ¥³¡¾°ÔÚ³¡¾°Õ»ÖÐ
    Director::getInstance()->pushScene(SelectionScene::create());
    //Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MatchingScene::createScene()));
    //Director::getInstance()->replaceScene(TransitionFade::create(0.2f, SelectionScene::createScene()));

}

// adventureGame ÈË»ú¶ÔÕ½Ä£Ê½°´Å¥»Øµ÷
void MainScene::adventureGameCallback(Ref* pSender)
{
    //Director::getInstance()->replaceScene(TransitionFade::create(0.2f, BoardSceneOffline::createScene()));
     // Ê¹ÓÃ pushScene£¬±£ÁôÖ÷²Ëµ¥³¡¾°ÔÚ³¡¾°Õ»ÖÐ
    Director::getInstance()->pushScene(BoardSceneOffline::create());
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
}

// collection ×Ô¶¨Òå¿¨×é°´Å¥»Øµ÷
void MainScene::collectionCallback(Ref* pSender)
{
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
    // Ê¹ÓÃ pushScene£¬±£ÁôÖ÷²Ëµ¥³¡¾°ÔÚ³¡¾°Õ»ÖÐ
    Director::getInstance()->pushScene(EditGroupScene::create());
}

// set ÉèÖÃ½çÃæ°´Å¥»Øµ÷
void MainScene::setCallback(Ref* pSender)
{
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
    Director::getInstance()->pushScene(SetScene::create());
}
