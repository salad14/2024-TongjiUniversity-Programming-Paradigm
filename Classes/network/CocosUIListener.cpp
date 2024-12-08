#include "CocosUIListener.h"
#include "cocos2d.h"
#include <codecvt>
#include <locale>

USING_NS_CC;

CocosUIListener::CocosUIListener()
    : logLabel(nullptr)
{
}

CocosUIListener::~CocosUIListener()
{
    // 资源由Cocos2d-x自动管理
}

// CocosUIListener.cpp

#include "CocosUIListener.h"
#include "cocos2d.h"

void CocosUIListener::initializeLogLabel(cocos2d::Layer* layer, const cocos2d::Vec2& position)
{
    // 获取可见尺寸以设定标签宽度
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    // 创建具有更大字体大小（例如30）的标签，设置固定宽度，自动高度
    logLabel = cocos2d::Label::createWithSystemFont(
        "",                  // 初始文本为空
        "Arial",             // 字体
        30,                  // 字体大小
        cocos2d::Size(visibleSize.width - 40, 0), // 固定宽度，自动高度
        cocos2d::TextHAlignment::CENTER,        // 水平居中对齐
        cocos2d::TextVAlignment::TOP            // 垂直顶端对齐
    );

    if (logLabel == nullptr)
    {
        CCLOG("Failed to create logLabel.");
        return;
    }

    // 设置标签位置
    logLabel->setPosition(position);

    // 设置颜色和描边以增强可读性
    logLabel->setColor(cocos2d::Color3B::WHITE);
    logLabel->enableOutline(cocos2d::Color4B::BLACK, 2); // 黑色描边，描边宽度为2

    // 设置标签对齐方式
    logLabel->setAlignment(cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::TOP);

    // 添加标签到指定的层中，层级为10
    layer->addChild(logLabel, 10);
}

void CocosUIListener::writeString(const ExitGames::Common::JString& str)
{
    if (logLabel)
    {
        cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([this, str]() {
            // 将 JString 转换为 std::wstring
            std::wstring wstr = str.cstr();

            // 将 std::wstring 转换为 UTF-8 std::string
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::string utf8Str = converter.to_bytes(wstr);

            logLabel->setString(utf8Str);
            });
    }
}

bool CocosUIListener::anyKeyPressed(void) const
{
    // 在Cocos2d-x中，使用触摸或按钮事件，不需要键盘输入
    return false;
}

void CocosUIListener::onLibClosed(void)
{
    if (logLabel)
    {
        logLabel->setString("Library closed.");
    }
}
