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
    // 获取 Singleton 实例
    static CocosUIListener* getInstance();

    // 附加 logLabel 到指定的 Layer
    void attachToLayer(cocos2d::Layer* layer, const cocos2d::Vec2& position);

    // 重写 UIListener 方法
    virtual void writeString(const ExitGames::Common::JString& str) override;
    virtual bool anyKeyPressed(void) const override;
    virtual void onLibClosed(void) override;

private:
    // 私有构造函数和析构函数
    CocosUIListener();
    virtual ~CocosUIListener();

    // 禁止拷贝构造和赋值操作
    CocosUIListener(const CocosUIListener&) = delete;
    CocosUIListener& operator=(const CocosUIListener&) = delete;

    // 用于显示日志的 Label
    cocos2d::Label* logLabel;

    // 文件输出流（可选，用于记录日志到文件）
    std::ofstream logFileStream;
};
