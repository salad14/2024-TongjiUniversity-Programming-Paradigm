#pragma once
#include "cocos2d.h"

class MainScene : public cocos2d::Scene
{
public:
    //��������
    static cocos2d::Scene* createScene();

    //��ʼ������
    virtual bool init();

    void normalGameCallback(cocos2d::Ref* psender);
    void adventureGameCallback(cocos2d::Ref* psender);
    void collectionCallback(cocos2d::Ref* psender);

    // implement the "static create()" method manually
    CREATE_FUNC(MainScene);
};
