/****************************************************************
* �û�ƥ��ҳ���ʵ��
* Author: Lee
* ע�⣺��δ�����������������á���Ѱ���ֻ��ڣ�������ֻ��Ϊ��������Ԥ��ʵ��λ��
*       Ŀǰֻ�ڵ���ʱ���������ת����һ����
 ****************************************************************/

#include "MatchingScene.h"//������
#include "BoardScene.h"   //��һ����
#include "MainScene.h"
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



// ��������
Scene* MatchingScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MatchingScene::create();
    scene->addChild(layer);
    return scene;
}

// ��ʼ������
bool MatchingScene::init()
{
    // ��������
    if (!Scene::init()) {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ���ر���
    const auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    const auto background = Sprite::create("../Resources/Scenes/3MatchingScene.png");
    if (background == nullptr) {
        problemLoading("../Resources/Scenes/3MatchingScene.png");
        return false;
    }
    background->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2));
    this->addChild(background);

    // cancel��ť�����ز˵�������棩
    auto cancel = MenuItemImage::create("../Resources/button/cancel.png", "../Resources/button/cancelSelected.png", CC_CALLBACK_1(MatchingScene::cancelCallback, this));
    if (cancel == nullptr || cancel->getContentSize().width <= 0 || cancel->getContentSize().height <= 0)
    {
        problemLoading("'canel.png' and 'canelSelected.png'");
    }
    else
    {
        cancel->setPosition(Vec2(screenSize.width / 2 + MATCHING_SCENE_CANCEL_OFFSET_X, screenSize.height / 2 + MATCHING_SCENE_CANCEL_OFFSET_Y));
    }
    auto menu = Menu::create(cancel, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // ���ü�ʱ��
    /*this->scheduleOnce([](float dt)
        {
        auto transition = cocos2d::TransitionFade::create(SCENE_TRANSITION_DURATION, BoardScene::createScene(), cocos2d::Color3B::WHITE);
        cocos2d::Director::getInstance()->replaceScene(transition);
        }, MATCHING_SCENE_DURATION + SCENE_TRANSITION_DURATION, "StartupSceneToInitialScene");*/

    return true;
}

// ȡ����ť�ص����� 
//���ܣ�ȡ��ƥ�䣬�ص����˵�

void MatchingScene::cancelCallback(Ref* pSender)
{
    // ���ص����Ч
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene()));

    //ע���˴������������ص�ֹͣ����


}