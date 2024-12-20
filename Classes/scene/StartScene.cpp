/****************************************************************
* ��ʼ����ҳ���ʵ��
* Author: Lee
 ****************************************************************/


#include "StartScene.h"
#include "HelloWorldScene.h"
#include "ui/CocosGUI.h"
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"

 // �����ռ�
USING_NS_CC;
// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// ��ʼ����Ƶ��������
int backgroundMusicSign = DEFAULT_MUSIC_SIGN;          // �������ֵ�ȱʡ��ʾ
int soundEffectSign = DEFAULT_MUSIC_SIGN;              // ���������Ч��ȱʡ��ʾ
float backgroundMusicVolumn = DEFAULT_MUSIC_VOLUMN;    // �������ֵ�����
float soundEffectVolumn = DEFAULT_MUSIC_VOLUMN;        // ������Ч������

/****************************************************************
* ��Ƶ���淽��
* Author: Lee
* �������ֺ͵��������Ч���� �ɲ���isLoop����
 ****************************************************************/
void audioPlayer(const std::string& audioPath, bool isLoop)
{
    if (isLoop) {
        // ����Ǳ������ֲ���֮ǰ�б��������ڲ���
        if (backgroundMusicSign != DEFAULT_MUSIC_SIGN) {
            // ֹͣ��ǰ�ı�������
            cocos2d::experimental::AudioEngine::stop(backgroundMusicSign);
        }
        // �����µı�������
        backgroundMusicSign = cocos2d::experimental::AudioEngine::play2d(audioPath, isLoop);
        cocos2d::experimental::AudioEngine::setVolume(backgroundMusicSign, backgroundMusicVolumn);
    }
    else {// �����ǵ��������Ч
        soundEffectSign = cocos2d::experimental::AudioEngine::play2d(audioPath, isLoop);
        cocos2d::experimental::AudioEngine::setVolume(soundEffectSign, soundEffectVolumn * 50);
    }
}

// ��������
Scene* StartScene::createScene()
{
    auto scene = Scene::create();
    auto layer = StartScene::create();
    scene->addChild(layer);
    return scene;
}

// ��ʼ������
bool StartScene::init()
{
    // ��������
    if (!Scene::init()) {
        return false;
    }

    // ��������
    audioPlayer("Music/MainScene.mp3", true);

    // ���ر���
    const auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    const auto background = Sprite::create("../Resources/Scenes/0StartScene.png");
    if (background == nullptr) {
        problemLoading("../Resources/Scenes/0StartScene.png");
        return false;
    }
    background->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2));
    this->addChild(background);

    // ����������
    auto progressBar = cocos2d::ui::LoadingBar::create("../Resources/ImageElements/StartupLoadingBar.png");
    progressBar->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2 + STARTUP_SCENE_LOADINGBAR_OFFSET_Y));
    progressBar->setPercent(0);
    this->addChild(progressBar);

    // ������ǩ����ʾ����
    auto progressLabel = Label::createWithTTF("0%", "../Resources/Fonts/arial.ttf", STARTUP_SCENE_FONT_SIZE);
    progressLabel->setPosition(Vec2(progressBar->getPosition().x - progressBar->getContentSize().width / 2 + STARTUP_SCENE_LOADING_LABEL_OFFSET_X, progressBar->getPosition().y));
    progressLabel->setVisible(false);
    this->addChild(progressLabel);

    // ���½������ͱ�ǩ
    constexpr float interval = STARTUP_SCENE_DURATION / 100.0f; // ÿ 1% ����ʱ��
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

    // ���ü�ʱ��
    this->scheduleOnce([](float dt) {
        auto transition = cocos2d::TransitionFade::create(SCENE_TRANSITION_DURATION, HelloWorld::createScene(), cocos2d::Color3B::WHITE);
        cocos2d::Director::getInstance()->replaceScene(transition);
        }, STARTUP_SCENE_DURATION + SCENE_TRANSITION_DURATION, "StartupSceneToInitialScene");

    return true;
}