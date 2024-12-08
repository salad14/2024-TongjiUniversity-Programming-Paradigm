/****************************************************************
* 主菜单页面的定义
* Author: Lee
 ****************************************************************/

#include "cocos2d.h"
#include "network/Photon_lib.h"
#include "network/CocosUIListener.h"

class MainScene : public cocos2d::Scene
{
public:
    // 创建场景
    static cocos2d::Scene* createScene();

    // 定期更新Photon
    void updatePhoton(float dt);

    // 初始化场景
    virtual bool init();

    // 按钮回调函数
    void normalGameCallback(cocos2d::Ref* psender);
    void adventureGameCallback(cocos2d::Ref* psender);
    void collectionCallback(cocos2d::Ref* psender);

    // 构造函数和析构函数
    MainScene();
    virtual ~MainScene();

    // implement the "static create()" method manually
    CREATE_FUNC(MainScene);

private:
    // PhotonLib 实例
    PhotonLib* photonLib;

    // CocosUIListener 实例
    CocosUIListener* cocosUIListener;
};
