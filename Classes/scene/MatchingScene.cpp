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
#include "network/Photon_lib.h"

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
    // ���ø����ʼ��
    if (!Scene::init()) {
        return false;
    }

    // ���ر���
    const auto screenSize = Director::getInstance()->getVisibleSize();
    const auto background = Sprite::create("../Resources/Scenes/3MatchingScene.png");
    if (background == nullptr) {
        problemLoading("../Resources/Scenes/3MatchingScene.png");
        return false;
    }
    background->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2));
    this->addChild(background);

    // ������������ǩ
    playerCountLabel = Label::createWithSystemFont("�ȴ���Ҽ���: 1/2", "Arial", 24);
    if (playerCountLabel)
    {
        playerCountLabel->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2 - 50));
        this->addChild(playerCountLabel);
    }

    // ��ȡ PhotonLib ʵ����������������仯�Ļص�
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (photonLib)
    {
        photonLib->setPlayerCountChangedCallback([=](int playerCount) {
            this->updatePlayerCount(playerCount);
            });
    }

    return true;
}

// �����������������Ƿ��л�����
void MatchingScene::updatePlayerCount(int playerCount)
{
    CCLOG("Current player count: %d", playerCount);

    // �������������ǩ
    if (playerCountLabel)
    {
        std::string text = "Wating Player: " + std::to_string(playerCount) + "/2";
        playerCountLabel->setString(text);
    }

    if (playerCount >= 2)
    {
        CCLOG("Player count reached 2. Switching to BoardScene.");
        // �л��� BoardScene
        auto transition = TransitionFade::create(0.2f, BoardScene::createScene(), Color3B::WHITE);
        Director::getInstance()->replaceScene(transition);
    }
}