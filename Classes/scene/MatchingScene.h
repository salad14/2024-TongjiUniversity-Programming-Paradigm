/****************************************************************
* �û�ƥ��ҳ��Ķ���
* Author: Lee
 ****************************************************************/

#ifndef _MATCHING_SCENE_H_
#define _MATCHING_SCENE_H_

#include "cocos2d.h"

class MatchingScene : public cocos2d::Scene {
public:
    // ��������
    static cocos2d::Scene* createScene();
    void cancelCallback(cocos2d::Ref* psender);// ���ذ�ť�Ļص�����

    // ��ʼ������
    virtual bool init();

    CREATE_FUNC(MatchingScene);
};

#endif // !_MATCHING_SCENE_H_