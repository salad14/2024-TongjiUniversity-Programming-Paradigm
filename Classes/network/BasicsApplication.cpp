// BasicsApplication.cpp
#include "cocos2d.h"                   // ȷ������ Cocos2d-x ͷ�ļ�
#include "Common-cpp/inc/Common.h"
#include "BasicsApplication.h"
#include "Photon_lib.h"
#include "UIListener.h"
#include "scene/HelloWorldScene.h"           // ���� HelloWorldScene ��ͷ�ļ�

using namespace cocos2d;                // ʹ�� Cocos2d-x �����ռ�
using namespace ExitGames::Common;

void BasicsApplication::run(UIListener* listener)
{
    // ��ʼ��PhotonLib����
    PhotonLib::initialize(listener);

    // ��ȡ����ʵ��
    PhotonLib* lib = PhotonLib::getInstance();
    if (!lib)
    {
        listener->writeString(L"PhotonLib initialization failed!");
        return;
    }

    // ���� HelloWorld ����
    auto scene = HelloWorld::createScene();
    if (!scene)
    {
        listener->writeString(L"Failed to create HelloWorld scene!");
        return;
    }

    // ���г���
    Director::getInstance()->runWithScene(scene);

    // ��ѭ����Cocos2d-x��������Ҫ�ֶ�����
}
