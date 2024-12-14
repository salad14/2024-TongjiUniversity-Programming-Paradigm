/****************************************************************
* ��Ϸ���ý����ʵ��
* Author: Lee
* ע��Ŀǰֻʵ������Ч����
 ****************************************************************/

#include "SetScene.h"
#include "MainScene.h"
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"
#include "ui/CocosGUI.h"
USING_NS_CC;

 // ��������
Scene* SetScene::createScene()
{
    auto scene = Scene::create();
    auto layer = SetScene::create();
    scene->addChild(layer);
    return scene;
}

// ��ʼ������
bool SetScene::init()
{
    // ��������
    if (!Scene::init()) {
        return false;
    }

    // ��������
    // audioPlayer("../Resources/Music/SettingsScene_Starlight.mp3", true);

    // ���ر���
    const auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    const auto background = Sprite::create("../Resources/Scenes/SetScene.png");
    background->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2));
    this->addChild(background);

    // ����������
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

    // Ϊ����������¼�������
    backgroundMusicVolumnSlider->addEventListener([=](Ref* sender, cocos2d::ui::Slider::EventType type) {
        if (type == cocos2d::ui::Slider::EventType::ON_SLIDEBALL_DOWN) {
            // ���ص����Ч
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
            // ���ص����Ч
            audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
        }
        if (type == cocos2d::ui::Slider::EventType::ON_PERCENTAGE_CHANGED) {
            const cocos2d::ui::Slider* effectVolumnslider = dynamic_cast<cocos2d::ui::Slider*>(sender);
            const float percent = effectVolumnslider->getPercent();
            soundEffectVolumn = percent / 100.0f;
            cocos2d::experimental::AudioEngine::setVolume(soundEffectSign, soundEffectVolumn);
        }
        });

    // �����������������
    this->addChild(backgroundMusicVolumnSlider);
    this->addChild(effectVolumnslider);

    // �������ذ�ť
    auto cancel = MenuItemImage::create("button/cancel.png", "button/cancelSelected.png", CC_CALLBACK_1(SetScene::cancelCallback, this));
    cancel->setPosition(Vec2(SETSCENE_RETURN_BUTTON_X, SETSCENE_RETURN_BUTTON_Y));
    auto menu = Menu::create(cancel, nullptr);
    // ��Ӳ˵�
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);

    return true;
}

//�������˵�
void SetScene::cancelCallback(Ref* pSender)
{
    // ���ص����Ч
    audioPlayer("Music/ClickSoundEffect.mp3", false);
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene()));
}