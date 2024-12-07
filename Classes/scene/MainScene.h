// MainScene.h
#pragma once
#include "cocos2d.h"
#include "network/Photon_lib.h"
#include "network/CocosUIListener.h"

class MainScene : public cocos2d::Scene
{
public:
    // 创建场景
    static cocos2d::Scene* createScene();

    // 初始化场景
    virtual bool init();

    // 按钮回调函数
    void normalGameCallback(cocos2d::Ref* psender);
    void adventureGameCallback(cocos2d::Ref* psender);
    void collectionCallback(cocos2d::Ref* psender);

    // implement the "static create()" method manually
    CREATE_FUNC(MainScene);

private:
    // PhotonLib 实例
    PhotonLib* photonLib;

    // CocosUIListener 实例
    CocosUIListener* cocosUIListener;

    // 定时器回调
    void updatePhoton(float dt);
};
