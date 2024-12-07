/****************************************************************
* 主菜单页面的定义
* Author: Lee
 ****************************************************************/

#include "cocos2d.h"

class MainScene : public cocos2d::Scene
{
public:
    //创建场景
    static cocos2d::Scene* createScene();

    //初始化场景
    virtual bool init();

    void normalGameCallback(cocos2d::Ref* psender);
    void adventureGameCallback(cocos2d::Ref* psender);
    void collectionCallback(cocos2d::Ref* psender);

    // implement the "static create()" method manually
    CREATE_FUNC(MainScene);
};
