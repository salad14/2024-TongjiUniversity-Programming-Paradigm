// MatchingScene.cpp

#include "MatchingScene.h" // 本场景
#include "BoardScene.h"    // 下一场景
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"
#include "network/CocosUIListener.h"
#include "network/Photon_lib.h"

USING_NS_CC;

// Print useful error message instead of segfaulting when files are not there.
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

    // 添加玩家数量标签
    playerCountLabel = Label::createWithSystemFont("Waitng Player: 1/2", "Arial", 24);
    if (playerCountLabel)
    {
        playerCountLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 50));
        this->addChild(playerCountLabel);
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

        // 设置玩家数量变化的回调
        photonLib->setPlayerCountChangedCallback([=](int playerCount) {
            // 更新玩家数量标签
            this->updatePlayerCount(playerCount);

            // 如果达到2人，则切换到 BoardScene
            if (playerCount >= 2)
            {
                CCLOG("Player count reached 2. Switching to BoardScene.");
                cocosUIListener->writeString(L"Player count reached 2. Starting game...");
                // 切换到 BoardScene
                auto transition = TransitionFade::create(0.2f, BoardScene::createScene(), Color3B::WHITE);
                Director::getInstance()->replaceScene(transition);
            }
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
