/****************************************************************
 * 加载完成开始界面定义
 * Author:  Lee
 * 注：利用了系统初始化时遗留的Helloworld场景文件，请注意名称区分
 ****************************************************************/

#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "network/CocosUIListener.h"
#include "network/Photon_lib.h"

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();

    // 点击按钮的回调
    void menuCloseCallback(cocos2d::Ref* pSender);
    void menuReplaceCallback(cocos2d::Ref* pSender);

    // 创建CocosUIListener
    CocosUIListener* cocosUIListener;
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
};

#endif // __HELLOWORLD_SCENE_H__
