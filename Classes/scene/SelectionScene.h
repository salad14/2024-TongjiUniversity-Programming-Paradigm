/****************************************************************
 * ��Ϸǰ����ѡ�����Ķ���
 * Author:   Lee
 ****************************************************************/

#pragma once
#ifndef _SELECTION_SCENE_H_
#define _SELECTION_SCENE_H_

#include "cocos2d.h"

USING_NS_CC;
 
class SelectionScene : public cocos2d::Scene {
private:
    Sprite* hoveredGroup;     // �����ͣ�Ŀ���
    Sprite* m_previewSprite;  // ָ���Ҳ�Ԥ��ͼ
    Label* m_groupNameLabel;  // �������Ʊ�ǩ
    Vector<Sprite*> m_groupIcons;  // �洢���п���ͼ��
    int m_selectedIndex;      // ��ǰѡ�еĿ�������
    void updatePreview(int index);
    std::map<Sprite*, bool> m_isHovered;  // ����ÿ��ͼ�����ͣ״̬
public:
    // ��������
    static cocos2d::Scene* createScene();

    // ��ʼ��
    virtual bool init();
    void SelectionScene::cancelCallback(Ref* pSender);
    void SelectionScene::startCallback(Ref* pSender);

    //�����ͣ���
    void SelectionScene::onMouseMove(Event* event);

    // ���Ŷ���
    void SelectionScene::scaleSprite(Sprite* sprite, float scale);

    // ʵ�� SelectionScene ��� create ����
    CREATE_FUNC(SelectionScene);
};

#endif // !_SELECTION_SCENE_H_