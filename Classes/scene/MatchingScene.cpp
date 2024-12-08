/****************************************************************
* 用户匹配页面的实现
* Author: Lee
* 注意：暂未加上真正的网络配置、搜寻对手环节，本场景只是为后续联机预留实现位置
*       目前只在倒计时后结束，跳转到下一场景
 ****************************************************************/

#include "MatchingScene.h"//本场景
#include "BoardScene.h"   //下一场景
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"
#include "network/Photon_lib.h"

// 命名空间
USING_NS_CC;
 // Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}



// 创建场景
Scene* MatchingScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MatchingScene::create();
    scene->addChild(layer);
    return scene;
}

// 初始化场景
bool MatchingScene::init()
{
    // 调用父类初始化
    if (!Scene::init()) {
        return false;
    }

    // 加载背景
    const auto screenSize = Director::getInstance()->getVisibleSize();
    const auto background = Sprite::create("../Resources/Scenes/3MatchingScene.png");
    if (background == nullptr) {
        problemLoading("../Resources/Scenes/3MatchingScene.png");
        return false;
    }
    background->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2));
    this->addChild(background);

    // 添加玩家数量标签
    playerCountLabel = Label::createWithSystemFont("等待玩家加入: 1/2", "Arial", 24);
    if (playerCountLabel)
    {
        playerCountLabel->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2 - 50));
        this->addChild(playerCountLabel);
    }

    // 获取 PhotonLib 实例并设置玩家数量变化的回调
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (photonLib)
    {
        photonLib->setPlayerCountChangedCallback([=](int playerCount) {
            this->updatePlayerCount(playerCount);
            });
    }

    return true;
}

// 更新玩家数量并检查是否切换场景
void MatchingScene::updatePlayerCount(int playerCount)
{
    CCLOG("Current player count: %d", playerCount);

    // 更新玩家数量标签
    if (playerCountLabel)
    {
        std::string text = "Wating Player: " + std::to_string(playerCount) + "/2";
        playerCountLabel->setString(text);
    }

    if (playerCount >= 2)
    {
        CCLOG("Player count reached 2. Switching to BoardScene.");
        // 切换到 BoardScene
        auto transition = TransitionFade::create(0.2f, BoardScene::createScene(), Color3B::WHITE);
        Director::getInstance()->replaceScene(transition);
    }
}