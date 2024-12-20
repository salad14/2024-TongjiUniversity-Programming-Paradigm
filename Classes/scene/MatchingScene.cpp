// MatchingScene.cpp

#include "MatchingScene.h" //本场景
#include "BoardScene.h"    //下一场景
#include "MainScene.h"
#include "players/GameData.h"      // 引入 GameData 头文件
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"
#include "network/CocosUIListener.h"
#include "network/Photon_lib.h"

USING_NS_CC;

// 辅助函数
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MatchingScene.cpp\n");
}

// 创建场景
Scene* MatchingScene::createScene()
{
    return MatchingScene::create();
}

// 初始化场景
bool MatchingScene::init()
{
    // 调用父类初始化
    if (!Scene::init()) {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建一个用于UI的Layer
    auto uiLayer = Layer::create();
    this->addChild(uiLayer);

    // 获取 Singleton 的 CocosUIListener 实例
    CocosUIListener* cocosUIListener = CocosUIListener::getInstance();

    // 将 logLabel 附加到当前 Layer 上
    cocosUIListener->attachToLayer(uiLayer, Vec2(visibleSize.width / 2, visibleSize.height - 50));

    // 加载背景
    const auto background = Sprite::create("../Resources/Scenes/3MatchingScene.png");
    if (background == nullptr) {
        problemLoading("../Resources/Scenes/3MatchingScene.png");
        return false;
    }
    background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    this->addChild(background);

    // 添加玩家数量标签，修正拼写错误
    playerCountLabel = Label::createWithSystemFont("Waiting Player: 1/2", "Arial", 24);
    if (playerCountLabel)
    {
        playerCountLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 50));
        this->addChild(playerCountLabel);
    }
    else
    {
        CCLOG("Failed to create playerCountLabel.");
    }

    // 获取 PhotonLib 实例并进行房间加入
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (photonLib)
    {
        // 设置房间加入后的回调，确认房间已加入
        photonLib->setRoomJoinedCallback([=]() {
            CCLOG("Room joined successfully.");
            cocosUIListener->writeString(L"Room joined successfully.");
            // 此处可以更新 UI，提示用户已加入房间
            });

        // 设置房间离开后的回调
        photonLib->setLeaveRoomCallback([=]() {
            CCLOG("Successfully left the room.");
            cocosUIListener->writeString(L"Successfully left the room.");
            // 如果需要，可以在这里执行更多操作，例如更新 UI 或记录日志
            });


        // 设置玩家数量变化的回调，并确保在主线程中执行
        photonLib->setPlayerCountChangedCallback([=](int playerCount) {
            // 将更新操作调度到主线程
            Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
                CCLOG("PlayerCountChangedCallback triggered with count: %d", playerCount);
                // 更新玩家数量标签
                this->updatePlayerCount(playerCount);

                // 如果达到2人，则切换到 BoardScene
                if (playerCount == 2 && !boardSceneCreated)
                {
                    boardSceneCreated = true;
                    CCLOG("Player count reached 2. Switching to BoardScene.");
                    cocosUIListener->writeString(L"Player count reached 2. Starting game...");

                    CCLOG("GameData instance starting");
                    // 初始化 GameData 单例（这将创建 player1 和 player2）
                    GameData& gameData = GameData::getInstance();
                    CCLOG("GameData instance initialized.");

                    // 确保 BoardScene 被正确创建
                    auto boardScene = BoardScene::createScene();
                    if (boardScene == nullptr) {
                        CCLOG("BoardScene::createScene() returned nullptr.");
                        return;
                    }
                    else {
                        CCLOG("BoardScene::createScene() succeeded.");
                    }

                    // 创建场景过渡
                    auto transition = TransitionFade::create(0.2f, boardScene, Color3B::WHITE);
                    if (transition == nullptr) {
                        CCLOG("TransitionFade::create() returned nullptr.");
                        return;
                    }
                    else {
                        CCLOG("TransitionFade::create() succeeded.");
                    }

                    // 切换到 BoardScene
                    Director::getInstance()->replaceScene(transition);
                }
                });
            });

        // 连接并加入/创建房间
        CCLOG("Joining or creating room from MatchingScene...");
        cocosUIListener->writeString(L"Joining or creating room...");
        photonLib->joinOrCreateRoom(ExitGames::Common::JString(L"DefaultRoom"));
    }
    else
    {
        CCLOG("PhotonLib is not initialized!");
        cocosUIListener->writeString(L"PhotonLib is not initialized!");
    }

    // 定期调用 PhotonLib::update()
    this->schedule(CC_SCHEDULE_SELECTOR(MatchingScene::updatePhoton), 0.1f); // 每0.1秒调用一次
    // cancel按钮（返回菜单界面界面）
    auto cancel = MenuItemImage::create("../Resources/button/cancel.png", "../Resources/button/cancelSelected.png", CC_CALLBACK_1(MatchingScene::cancelCallback, this));
    if (cancel == nullptr || cancel->getContentSize().width <= 0 || cancel->getContentSize().height <= 0)
    {
        problemLoading("'cancel.png' and 'cancelSelected.png'");
    }
    else
    {
        cancel->setPosition(Vec2(visibleSize.width / 2 + MATCHING_SCENE_CANCEL_OFFSET_X, visibleSize.height / 2 + MATCHING_SCENE_CANCEL_OFFSET_Y));
    }
    auto menu = Menu::create(cancel, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    return true;
}

// 定期更新Photon
void MatchingScene::updatePhoton(float dt)
{
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (photonLib)
    {
        photonLib->update();
    }
}

// 更新玩家数量并检查是否切换场景
void MatchingScene::updatePlayerCount(int playerCount)
{
    CCLOG("Current player count: %d", playerCount);

    // 更新玩家数量标签
    if (playerCountLabel)
    {
        std::string text = "Waiting Player: " + std::to_string(playerCount) + "/2";
        playerCountLabel->setString(text);
        CCLOG("playerCountLabel updated to: %s", text.c_str());
    }
    else
    {
        CCLOG("playerCountLabel is null.");
    }
}

// 取消按钮回调函数 
// 功能：取消匹配，回到主菜单
void MatchingScene::cancelCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);

    // 获取 PhotonLib 实例并离开房间
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (photonLib)
    {
        CCLOG("Cancelling matchmaking. Leaving room...");
        photonLib->leaveRoom();
        CocosUIListener::getInstance()->writeString(L"Cancelling matchmaking...");
    }
    else
    {
        CCLOG("PhotonLib is not initialized!");
        CocosUIListener::getInstance()->writeString(L"PhotonLib is not initialized!");
    }

    // 切换回主菜单
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene(), Color3B::WHITE));
}
