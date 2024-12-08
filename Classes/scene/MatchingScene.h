/****************************************************************
* �û�ƥ��ҳ��Ķ���
* Author: Lee
 ****************************************************************/

#ifndef _MATCHING_SCENE_H_
#define _MATCHING_SCENE_H_

#include "cocos2d.h"

class MatchingScene : public cocos2d::Scene
{
public:
    // ��������
    static cocos2d::Scene* createScene();

    // ��ʼ������
    virtual bool init();

    // ʵ�� "static create()" ����
    CREATE_FUNC(MatchingScene);

    // ������������ķ���
    void updatePlayerCount(int playerCount);

private:
    cocos2d::Label* playerCountLabel; // ��ʾ��������ı�ǩ
};

#endif // !_MATCHING_SCENE_H_