// MatchingScene.cpp

#include "MatchingScene.h" //������
#include "BoardScene.h"    //��һ����
#include "MainScene.h"
#include "players/GameData.h"      // ���� GameData ͷ�ļ�
#include "proj.win32/Alluse.h"
#include "proj.win32/AudioPlayer.h"
#include "network/CocosUIListener.h"
#include "network/Photon_lib.h"

USING_NS_CC;

// ��������
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

    // ������������ǩ������ƴд����
    playerCountLabel = Label::createWithSystemFont("Waiting Player: 1/2", "Arial", 24);
    if (playerCountLabel)
    {
        playerCountLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 50));
        this->addChild(playerCountLabel);
    }
    else
    {
        CCLOG("Failed to create playerCountLabel.");
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

        // ���÷����뿪��Ļص�
        photonLib->setLeaveRoomCallback([=]() {
            CCLOG("Successfully left the room.");
            cocosUIListener->writeString(L"Successfully left the room.");
            // �����Ҫ������������ִ�и��������������� UI ���¼��־
            });


        // ������������仯�Ļص�����ȷ�������߳���ִ��
        photonLib->setPlayerCountChangedCallback([=](int playerCount) {
            // �����²������ȵ����߳�
            Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
                CCLOG("PlayerCountChangedCallback triggered with count: %d", playerCount);
                // �������������ǩ
                this->updatePlayerCount(playerCount);

                // ����ﵽ2�ˣ����л��� BoardScene
                if (playerCount == 2 && !boardSceneCreated)
                {
                    boardSceneCreated = true;
                    CCLOG("Player count reached 2. Switching to BoardScene.");
                    cocosUIListener->writeString(L"Player count reached 2. Starting game...");

                    CCLOG("GameData instance starting");
                    // ��ʼ�� GameData �������⽫���� player1 �� player2��
                    GameData& gameData = GameData::getInstance();
                    CCLOG("GameData instance initialized.");

                    // ȷ�� BoardScene ����ȷ����
                    auto boardScene = BoardScene::createScene();
                    if (boardScene == nullptr) {
                        CCLOG("BoardScene::createScene() returned nullptr.");
                        return;
                    }
                    else {
                        CCLOG("BoardScene::createScene() succeeded.");
                    }

                    // ������������
                    auto transition = TransitionFade::create(0.2f, boardScene, Color3B::WHITE);
                    if (transition == nullptr) {
                        CCLOG("TransitionFade::create() returned nullptr.");
                        return;
                    }
                    else {
                        CCLOG("TransitionFade::create() succeeded.");
                    }

                    // �л��� BoardScene
                    Director::getInstance()->replaceScene(transition);
                }
                });
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
    // cancel��ť�����ز˵�������棩
    auto cancel = MenuItemImage::create("../Resources/button/cancel.png", "../Resources/button/cancelSelected.png", CC_CALLBACK_1(MatchingScene::cancelCallback, this));
    if (cancel == nullptr || cancel->getContentSize().width <= 0 || cancel->getContentSize().height <= 0)
    {
        problemLoading("'cancel.png' and 'cancelSelected.png'");
    }
    else
    {
        cancel->setPosition(Vec2(visibleSize.width / 2 + MATCHING_SCENE_CANCEL_OFFSET_X, visibleSize.height / 2 + MATCHING_SCENE_CANCEL_OFFSET_Y));
    }
    auto menu = Menu::create(cancel, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

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

// ȡ����ť�ص����� 
// ���ܣ�ȡ��ƥ�䣬�ص����˵�
void MatchingScene::cancelCallback(Ref* pSender)
{
    // ���ص����Ч
    audioPlayer("../Resources/Music/ClickSoundEffect.mp3", false);

    // ��ȡ PhotonLib ʵ�����뿪����
    PhotonLib* photonLib = PhotonLib::getInstance();
    if (photonLib)
    {
        CCLOG("Cancelling matchmaking. Leaving room...");
        photonLib->leaveRoom();
        CocosUIListener::getInstance()->writeString(L"Cancelling matchmaking...");
    }
    else
    {
        CCLOG("PhotonLib is not initialized!");
        CocosUIListener::getInstance()->writeString(L"PhotonLib is not initialized!");
    }

    // �л������˵�
    Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::createScene(), Color3B::WHITE));
}
