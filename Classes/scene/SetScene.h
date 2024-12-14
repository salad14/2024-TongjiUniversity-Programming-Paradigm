/****************************************************************
* 游戏设置界面的定义
* Author: Lee
 ****************************************************************/
#include "cocos2d.h"


 /*
  * Class Name:     SettingsScene
  * Class Function: 设置场景类
  */
class SetScene : public cocos2d::Scene {
public:
    // 创建场景
    static cocos2d::Scene* createScene();

    // 初始化场景
    virtual bool init();

    void cancelCallback(Ref* pSender);// 取消按钮
    void SetScene::setCloseCallback(Ref* pSender);//退出游戏按钮

    // 实现 SettingsScene 类的 create 方法
    CREATE_FUNC(SetScene);
};