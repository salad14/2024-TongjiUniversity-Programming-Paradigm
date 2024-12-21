/****************************************************************
 * 游戏前卡组选择界面的定义
 * Author:   Lee
 ****************************************************************/

#pragma once
#ifndef _SELECTION_SCENE_H_
#define _SELECTION_SCENE_H_

#include "cocos2d.h"

USING_NS_CC;
 
class SelectionScene : public cocos2d::Scene {
private:
    Sprite* hoveredGroup;     // 鼠标悬停的卡组
    Sprite* m_previewSprite;  // 指向右侧预览图
    Label* m_groupNameLabel;  // 卡组名称标签
    Vector<Sprite*> m_groupIcons;  // 存储所有卡组图标
    int m_selectedIndex;      // 当前选中的卡组索引
    void updatePreview(int index);
    std::map<Sprite*, bool> m_isHovered;  // 跟踪每个图标的悬停状态
public:
    // 创建场景
    static cocos2d::Scene* createScene();

    // 初始化
    virtual bool init();
    void SelectionScene::cancelCallback(Ref* pSender);
    void SelectionScene::startCallback(Ref* pSender);

    //鼠标悬停检测
    void SelectionScene::onMouseMove(Event* event);

    // 缩放动画
    void SelectionScene::scaleSprite(Sprite* sprite, float scale);

    // 实现 SelectionScene 类的 create 方法
    CREATE_FUNC(SelectionScene);
};

#endif // !_SELECTION_SCENE_H_