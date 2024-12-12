// CocosUIListener.h
#pragma once

#include "UIListener.h"
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <fstream>
#include <string>

class CocosUIListener : public UIListener
{
public:
    // ��ȡ Singleton ʵ��
    static CocosUIListener* getInstance();

    // ���� logLabel ��ָ���� Layer
    void attachToLayer(cocos2d::Layer* layer, const cocos2d::Vec2& position);

    // ��д UIListener ����
    virtual void writeString(const ExitGames::Common::JString& str) override;
    virtual bool anyKeyPressed(void) const override;
    virtual void onLibClosed(void) override;

private:
    // ˽�й��캯������������
    CocosUIListener();
    virtual ~CocosUIListener();

    // ��ֹ��������͸�ֵ����
    CocosUIListener(const CocosUIListener&) = delete;
    CocosUIListener& operator=(const CocosUIListener&) = delete;

    // ������ʾ��־�� Label
    cocos2d::Label* logLabel;

    // �ļ����������ѡ�����ڼ�¼��־���ļ���
    std::ofstream logFileStream;
};
