/****************************************************************
* 起始加载页面的实现
* Author: Lee
 ****************************************************************/


#include "StartScene.h"
#include "HelloWorldScene.h"
#include "ui/CocosGUI.h"
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

// 初始的音频引擎设置
int backgroundMusicSign = DEFAULT_MUSIC_SIGN;          // 背景音乐的缺省表示
int soundEffectSign = DEFAULT_MUSIC_SIGN;              // 单个点击音效的缺省表示
float backgroundMusicVolumn = DEFAULT_MUSIC_VOLUMN;    // 背景音乐的音量
float soundEffectVolumn = DEFAULT_MUSIC_VOLUMN;        // 单次音效的音量

/****************************************************************
* 音频引擎方法
* Author: Lee
* 背景音乐和单个点击音效共用 由参数isLoop控制
 ****************************************************************/
void audioPlayer(const std::string& audioPath, bool isLoop)
{
    if (isLoop) {
        // 如果是背景音乐并且之前有背景音乐在播放
        if (backgroundMusicSign != DEFAULT_MUSIC_SIGN) {
            // 停止当前的背景音乐
            cocos2d::experimental::AudioEngine::stop(backgroundMusicSign);
        }
        // 播放新的背景音乐
        backgroundMusicSign = cocos2d::experimental::AudioEngine::play2d(audioPath, isLoop);
        cocos2d::experimental::AudioEngine::setVolume(backgroundMusicSign, backgroundMusicVolumn);
    }
    else {// 否则是单个点击音效
        soundEffectSign = cocos2d::experimental::AudioEngine::play2d(audioPath, isLoop);
        cocos2d::experimental::AudioEngine::setVolume(soundEffectSign, soundEffectVolumn * 50);
    }
}

// 创建场景
Scene* StartScene::createScene()
{
    auto scene = Scene::create();
    auto layer = StartScene::create();
    scene->addChild(layer);
    return scene;
}

// 初始化场景
bool StartScene::init()
{
    // 创建场景
    if (!Scene::init()) {
        return false;
    }

    // 加载音乐
    audioPlayer("Music/MainScene.mp3", true);

    // 加载背景
    const auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    const auto background = Sprite::create("../Resources/Scenes/0StartScene.png");
    if (background == nullptr) {
        problemLoading("../Resources/Scenes/0StartScene.png");
        return false;
    }
    background->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2));
    this->addChild(background);

    // 创建进度条
    auto progressBar = cocos2d::ui::LoadingBar::create("../Resources/ImageElements/StartupLoadingBar.png");
    progressBar->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2 + STARTUP_SCENE_LOADINGBAR_OFFSET_Y));
    progressBar->setPercent(0);
    this->addChild(progressBar);

    // 创建标签以显示进度
    auto progressLabel = Label::createWithTTF("0%", "../Resources/Fonts/arial.ttf", STARTUP_SCENE_FONT_SIZE);
    progressLabel->setPosition(Vec2(progressBar->getPosition().x - progressBar->getContentSize().width / 2 + STARTUP_SCENE_LOADING_LABEL_OFFSET_X, progressBar->getPosition().y));
    progressLabel->setVisible(false);
    this->addChild(progressLabel);

    // 更新进度条和标签
    constexpr float interval = STARTUP_SCENE_DURATION / 100.0f; // 每 1% 所需时间
    for (int i = 0; i <= 100; i++) {
        this->scheduleOnce([progressBar, progressLabel, i](float dt) {
            progressLabel->setVisible(i >= 10);
            progressBar->setPercent(i);
            progressLabel->setString(std::to_string(i) + "%");
            float percentage = progressBar->getPercent() / 100.0f;
            float xPosition = progressBar->getPosition().x - progressBar->getContentSize().width / 2 + progressBar->getContentSize().width * percentage + STARTUP_SCENE_LOADING_LABEL_OFFSET_X;
            progressLabel->setPosition(Vec2(xPosition, progressBar->getPosition().y));
            }, interval * i, "UpdateStartupLoadingBar" + std::to_string(i));
    }

    // 设置计时器
    this->scheduleOnce([](float dt) {
        auto transition = cocos2d::TransitionFade::create(SCENE_TRANSITION_DURATION, HelloWorld::createScene(), cocos2d::Color3B::WHITE);
        cocos2d::Director::getInstance()->replaceScene(transition);
        }, STARTUP_SCENE_DURATION + SCENE_TRANSITION_DURATION, "StartupSceneToInitialScene");

    return true;
}