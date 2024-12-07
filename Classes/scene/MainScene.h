// MainScene.h
#pragma once
#include "cocos2d.h"
#include "network/Photon_lib.h"
#include "network/CocosUIListener.h"

class MainScene : public cocos2d::Scene
{
public:
    // ��������
    static cocos2d::Scene* createScene();

    // ��ʼ������
    virtual bool init();

    // ��ť�ص�����
    void normalGameCallback(cocos2d::Ref* psender);
    void adventureGameCallback(cocos2d::Ref* psender);
    void collectionCallback(cocos2d::Ref* psender);

    // implement the "static create()" method manually
    CREATE_FUNC(MainScene);

private:
    // PhotonLib ʵ��
    PhotonLib* photonLib;

    // CocosUIListener ʵ��
    CocosUIListener* cocosUIListener;

    // ��ʱ���ص�
    void updatePhoton(float dt);
};
