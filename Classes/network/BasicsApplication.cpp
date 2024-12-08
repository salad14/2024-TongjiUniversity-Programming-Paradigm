// BasicsApplication.cpp
#include "cocos2d.h"                   // 确保包含 Cocos2d-x 头文件
#include "Common-cpp/inc/Common.h"
#include "BasicsApplication.h"
#include "Photon_lib.h"
#include "UIListener.h"
#include "scene/HelloWorldScene.h"           // 包含 HelloWorldScene 的头文件

using namespace cocos2d;                // 使用 Cocos2d-x 命名空间
using namespace ExitGames::Common;

void BasicsApplication::run(UIListener* listener)
{
    // 初始化PhotonLib单例
    PhotonLib::initialize(listener);

    // 获取单例实例
    PhotonLib* lib = PhotonLib::getInstance();
    if (!lib)
    {
        listener->writeString(L"PhotonLib initialization failed!");
        return;
    }

    // 创建 HelloWorld 场景
    auto scene = HelloWorld::createScene();
    if (!scene)
    {
        listener->writeString(L"Failed to create HelloWorld scene!");
        return;
    }

    // 运行场景
    Director::getInstance()->runWithScene(scene);

    // 主循环由Cocos2d-x管理，不需要手动处理
}
