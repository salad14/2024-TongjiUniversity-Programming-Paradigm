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
    CocosUIListener();
    virtual ~CocosUIListener();

    // 初始化日志标签
    void initializeLogLabel(cocos2d::Layer* layer, const cocos2d::Vec2& position);

    // Override UIListener methods
    virtual void writeString(const ExitGames::Common::JString& str) override;
    virtual bool anyKeyPressed(void) const override;
    virtual void onLibClosed(void) override;

private:
    // 用于显示日志的Label
    cocos2d::Label* logLabel;
    std::ofstream logFileStream; // 文件输出流
};
