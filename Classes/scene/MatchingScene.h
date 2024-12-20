// MatchingScene.h

#ifndef __MATCHING_SCENE_H__
#define __MATCHING_SCENE_H__

#include "cocos2d.h"

class MatchingScene : public cocos2d::Scene
{
public:
    // 创建场景
    static cocos2d::Scene* createScene();
    void cancelCallback(cocos2d::Ref* psender);// 返回按钮的回调函数

    // 初始化方法
    virtual bool init();

    // 定期更新Photon
    void updatePhoton(float dt);

    // 更新玩家数量并检查是否切换场景
    void updatePlayerCount(int playerCount);

    std::atomic<bool> boardSceneCreated;

    // 实现 "static create()" 方法
    CREATE_FUNC(MatchingScene);

private:
    cocos2d::Label* playerCountLabel;
};

#endif // __MATCHING_SCENE_H__
