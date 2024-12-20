// MatchingScene.h

#ifndef __MATCHING_SCENE_H__
#define __MATCHING_SCENE_H__

#include "cocos2d.h"

class MatchingScene : public cocos2d::Scene
{
public:
    // ��������
    static cocos2d::Scene* createScene();
    void cancelCallback(cocos2d::Ref* psender);// ���ذ�ť�Ļص�����

    // ��ʼ������
    virtual bool init();

    // ���ڸ���Photon
    void updatePhoton(float dt);

    // �����������������Ƿ��л�����
    void updatePlayerCount(int playerCount);

    std::atomic<bool> boardSceneCreated;

    // ʵ�� "static create()" ����
    CREATE_FUNC(MatchingScene);

private:
    cocos2d::Label* playerCountLabel;
};

#endif // __MATCHING_SCENE_H__
