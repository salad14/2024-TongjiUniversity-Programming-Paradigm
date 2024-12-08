/****************************************************************
* �û�ƥ��ҳ���ʵ��
* Author: Lee
* ע�⣺��δ�����������������á���Ѱ���ֻ��ڣ�������ֻ��Ϊ��������Ԥ��ʵ��λ��
*       Ŀǰֻ�ڵ���ʱ���������ת����һ����
 ****************************************************************/

#include "MatchingScene.h"//������
#include "BoardScene.h"   //��һ����
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

    

    // ���ر���
    const auto screenSize = cocos2d::Director::getInstance()->getVisibleSize();
    const auto background = Sprite::create("../Resources/Scenes/3MatchingScene.png");
    if (background == nullptr) {
        problemLoading("../Resources/Scenes/3MatchingScene.png");
        return false;
    }
    background->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2));
    this->addChild(background);

    // ���ü�ʱ��
    this->scheduleOnce([](float dt) {
        auto transition = cocos2d::TransitionFade::create(SCENE_TRANSITION_DURATION, BoardScene::createScene(), cocos2d::Color3B::WHITE);
        cocos2d::Director::getInstance()->replaceScene(transition);
        }, MATCHING_SCENE_DURATION + SCENE_TRANSITION_DURATION, "StartupSceneToInitialScene");

    return true;
}