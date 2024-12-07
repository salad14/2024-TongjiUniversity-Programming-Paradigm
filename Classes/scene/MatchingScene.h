/****************************************************************
* 用户匹配页面的定义
* Author: Lee
 ****************************************************************/

#ifndef _MATCHING_SCENE_H_
#define _MATCHING_SCENE_H_

#include "cocos2d.h"

class MatchingScene : public cocos2d::Scene {
public:
    // 创建场景
    static cocos2d::Scene* createScene();

    // 初始化场景
    virtual bool init();

    CREATE_FUNC(MatchingScene);
};

#endif // !_MATCHING_SCENE_H_