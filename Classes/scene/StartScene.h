/****************************************************************
 * 起始加载页面的定义
 * Author:  Lee
 ****************************************************************/
#ifndef _STARTUP_SCENE_H_
#define _STARTUP_SCENE_H_

#include "cocos2d.h"

 /*
  * Class Name:     StartupScene
  * Class Function: 启动场景类
  */
class StartScene : public cocos2d::Scene {
public:
    // 创建场景
    static cocos2d::Scene* createScene();

    // 初始化场景
    virtual bool init();

    CREATE_FUNC(StartScene);
};

#endif // !_STARTUP_SCENE_H_