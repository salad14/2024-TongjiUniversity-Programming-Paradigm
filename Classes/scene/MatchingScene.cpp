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
    // 创建场景
    if (!Scene::init()) {
        return false;
    }

    

    // 加载背景
    const auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    const auto background = Sprite::create("../Resources/Scenes/3MatchingScene.png");
    if (background == nullptr) {
        problemLoading("../Resources/Scenes/3MatchingScene.png");
        return false;
    }
    background->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2));
    this->addChild(background);

    // 设置计时器
    this->scheduleOnce([](float dt) {
        auto transition = cocos2d::TransitionFade::create(SCENE_TRANSITION_DURATION, BoardScene::createScene(), cocos2d::Color3B::WHITE);
        cocos2d::Director::getInstance()->replaceScene(transition);
        }, MATCHING_SCENE_DURATION + SCENE_TRANSITION_DURATION, "StartupSceneToInitialScene");

    return true;
}