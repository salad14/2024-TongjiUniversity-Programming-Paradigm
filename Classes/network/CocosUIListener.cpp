#include "CocosUIListener.h"
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

void CocosUIListener::initializeLogLabel(cocos2d::Layer* layer, const cocos2d::Vec2& position)
{
    logLabel = Label::createWithSystemFont("", "Arial", 20);
    logLabel->setPosition(position);
    logLabel->setColor(Color3B::WHITE);
    logLabel->enableOutline(Color4B::BLACK, 2); // 添加黑色描边以增强可读性
    layer->addChild(logLabel, 10);
}

void CocosUIListener::writeString(const ExitGames::Common::JString& str)
{
    if (logLabel)
    {
        // 将 JString 转换为 std::wstring
        std::wstring wstr = str.cstr();

        // 将 std::wstring 转换为 UTF-8 std::string
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string utf8Str = converter.to_bytes(wstr);

        logLabel->setString(utf8Str);
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
