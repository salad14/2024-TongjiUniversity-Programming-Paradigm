/****************************************************************
 * ��ʼ����ҳ��Ķ���
 * Author:  Lee
 ****************************************************************/
#ifndef _STARTUP_SCENE_H_
#define _STARTUP_SCENE_H_

#include "cocos2d.h"

 /*
  * Class Name:     StartupScene
  * Class Function: ����������
  */
class StartScene : public cocos2d::Scene {
public:
    // ��������
    static cocos2d::Scene* createScene();

    // ��ʼ������
    virtual bool init();

    CREATE_FUNC(StartScene);
};

#endif // !_STARTUP_SCENE_H_