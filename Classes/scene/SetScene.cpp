/****************************************************************
* 游戏设置界面的实现
* Author: Lee
* 注：目前只实现了音效设置
 ****************************************************************/

#include "SetScene.h"
#include "MainScene.h"
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"
#include "ui/CocosGUI.h"
USING_NS_CC;

 // 创建场景
Scene* SetScene::createScene()
{
    auto scene = Scene::create();
    auto layer = SetScene::create();
    scene->addChild(layer);
    return scene;
}

// 初始化场景
bool SetScene::init()
{
    // 创建场景
    if (!Scene::init()) {
        return false;
    }

    // 加载音乐
    // audioPlayer("../Resources/Music/SettingsScene_Starlight.mp3", true);

    // 加载背景
    const auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    const auto background = Sprite::create("../Resources/Scenes/SetScene.png");
    background->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2));
    this->addChild(background);

    // 创建滑动条
    auto backgroundMusicVolumnSlider = cocos2d::ui::Slider::create();
    backgroundMusicVolumnSlider->loadBarTexture("../Resources/ImageElements/SliderBack.png");
    backgroundMusicVolumnSlider->loadSlidBallTextures("../Resources/ImageElements/SliderNodeNormal.png",
        "../Resources/ImageElements/SliderNodePress.png",
        "../Resources/ImageElements/SliderNodeDisable.png");
    backgroundMusicVolumnSlider->loadProgressBarTexture("../Resources/ImageElements/SliderBar.png");
    backgroundMusicVolumnSlider->setPosition(cocos2d::Vec2(SETSCENE_SLIDER_X, SETSCENE_BGMSLIDER_Y));
    backgroundMusicVolumnSlider->setPercent(backgroundMusicVolumn * 100);
    auto effectVolumnslider = cocos2d::ui::Slider::create();
    effectVolumnslider->loadBarTexture("../Resources/ImageElements/SliderBack.png");
    effectVolumnslider->loadSlidBallTextures("../Resources/ImageElements/SliderNodeNormal.png",
        "../Resources/ImageElements/SliderNodePress.png",
        "../Resources/ImageElements/SliderNodeDisable.png");
    effectVolumnslider->loadProgressBarTexture("../Resources/ImageElements/SliderBar.png");
    effectVolumnslider->setPosition(cocos2d::Vec2(SETSCENE_SLIDER_X, SETSCENE_EFFSLIDER_Y));
    effectVolumnslider->setPercent(soundEffectVolumn * 100);

    // 为滑动条添加事件处理器
    backgroundMusicVolumnSlider->addEventListener([=](Ref* sender, cocos2d::ui::Slider::EventType type) {
        if (type == cocos2d::ui::Slider::EventType::ON_SLIDEBALL_DOWN) {
            // 加载点击音效
            audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
        }
        if (type == cocos2d::ui::Slider::EventType::ON_PERCENTAGE_CHANGED) {
            const cocos2d::ui::Slider* backgroundMusicVolumnSlider = dynamic_cast<cocos2d::ui::Slider*>(sender);
            const float percent = backgroundMusicVolumnSlider->getPercent();
            backgroundMusicVolumn = percent / 100.0f;
            cocos2d::experimental::AudioEngine::setVolume(backgroundMusicSign, backgroundMusicVolumn);
        }
        });
    effectVolumnslider->addEventListener([=](Ref* sender, cocos2d::ui::Slider::EventType type) {
        if (type == cocos2d::ui::Slider::EventType::ON_SLIDEBALL_DOWN) {
            // 加载点击音效
            audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
        }
        if (type == cocos2d::ui::Slider::EventType::ON_PERCENTAGE_CHANGED) {
            const cocos2d::ui::Slider* effectVolumnslider = dynamic_cast<cocos2d::ui::Slider*>(sender);
            const float percent = effectVolumnslider->getPercent();
            soundEffectVolumn = percent / 100.0f;
            cocos2d::experimental::AudioEngine::setVolume(soundEffectSign, soundEffectVolumn);
        }
        });

    // 将滑动条添加至场景
    this->addChild(backgroundMusicVolumnSlider);
    this->addChild(effectVolumnslider);

    // 创建返回按钮
    auto cancel = MenuItemImage::create("button/cancel.png", "button/cancelSelected.png", CC_CALLBACK_1(SetScene::cancelCallback, this));
    cancel->setPosition(Vec2(SETSCENE_RETURN_BUTTON_X, SETSCENE_RETURN_BUTTON_Y));
    auto menu = Menu::create(cancel, nullptr);
    // 添加菜单
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);

    return true;
}

//返回主菜单
void SetScene::cancelCallback(Ref* pSender)
{
    // 加载点击音效
    audioPlayer("Music/ClickSoundEffect.mp3", false);
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene()));
}