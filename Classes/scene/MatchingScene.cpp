// MatchingScene.cpp

#include "MatchingScene.h" // ������
#include "BoardScene.h"    // ��һ����
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

// ��������
Scene* MatchingScene::createScene()
{
    return MatchingScene::create();
}

// ��ʼ������
bool MatchingScene::init()
{
    // ���ø����ʼ��
    if (!Scene::init()) {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ����һ������UI��Layer
    auto uiLayer = Layer::create();
    this->addChild(uiLayer);

    // ��ȡ Singleton �� CocosUIListener ʵ��
    CocosUIListener* cocosUIListener = CocosUIListener::getInstance();

    // �� logLabel ���ӵ���ǰ Layer ��
    cocosUIListener->attachToLayer(uiLayer, Vec2(visibleSize.width / 2, visibleSize.height - 50));

    // ���ر���
    const auto background = Sprite::create("../Resources/Scenes/3MatchingScene.png");
    if (background == nullptr) {
        problemLoading("../Resources/Scenes/3MatchingScene.png");
        return false;
    }
    background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    this->addChild(background);

    // ������������ǩ
    playerCountLabel = Label::createWithSystemFont("Waitng Player: 1/2", "Arial", 24);
    if (playerCountLabel)
    {
        playerCountLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 50));
        this->addChild(playerCountLabel);
    }

    // ��ȡ PhotonLib ʵ�������з������
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (photonLib)
    {
        // ���÷�������Ļص���ȷ�Ϸ����Ѽ���
        photonLib->setRoomJoinedCallback([=]() {
            CCLOG("Room joined successfully.");
            cocosUIListener->writeString(L"Room joined successfully.");
            // �˴����Ը��� UI����ʾ�û��Ѽ��뷿��
            });

        // ������������仯�Ļص�
        photonLib->setPlayerCountChangedCallback([=](int playerCount) {
            // �������������ǩ
            this->updatePlayerCount(playerCount);

            // ����ﵽ2�ˣ����л��� BoardScene
            if (playerCount >= 2)
            {
                CCLOG("Player count reached 2. Switching to BoardScene.");
                cocosUIListener->writeString(L"Player count reached 2. Starting game...");
                // �л��� BoardScene
                auto transition = TransitionFade::create(0.2f, BoardScene::createScene(), Color3B::WHITE);
                Director::getInstance()->replaceScene(transition);
            }
            });

        // ���Ӳ�����/��������
        CCLOG("Joining or creating room from MatchingScene...");
        cocosUIListener->writeString(L"Joining or creating room...");
        photonLib->joinOrCreateRoom(ExitGames::Common::JString(L"DefaultRoom"));
    }
    else
    {
        CCLOG("PhotonLib is not initialized!");
        cocosUIListener->writeString(L"PhotonLib is not initialized!");
    }

    // ���ڵ��� PhotonLib::update()
    this->schedule(CC_SCHEDULE_SELECTOR(MatchingScene::updatePhoton), 0.1f); // ÿ0.1�����һ��

    return true;
}

// ���ڸ���Photon
void MatchingScene::updatePhoton(float dt)
{
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (photonLib)
    {
        photonLib->update();
    }
}

// �����������������Ƿ��л�����
void MatchingScene::updatePlayerCount(int playerCount)
{
    CCLOG("Current player count: %d", playerCount);

    // �������������ǩ
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
