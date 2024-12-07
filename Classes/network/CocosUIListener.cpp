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
    // ��Դ��Cocos2d-x�Զ�����
}

void CocosUIListener::initializeLogLabel(cocos2d::Layer* layer, const cocos2d::Vec2& position)
{
    logLabel = Label::createWithSystemFont("", "Arial", 20);
    logLabel->setPosition(position);
    logLabel->setColor(Color3B::WHITE);
    logLabel->enableOutline(Color4B::BLACK, 2); // ��Ӻ�ɫ�������ǿ�ɶ���
    layer->addChild(logLabel, 10);
}

void CocosUIListener::writeString(const ExitGames::Common::JString& str)
{
    if (logLabel)
    {
        // �� JString ת��Ϊ std::wstring
        std::wstring wstr = str.cstr();

        // �� std::wstring ת��Ϊ UTF-8 std::string
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string utf8Str = converter.to_bytes(wstr);

        logLabel->setString(utf8Str);
    }
}

bool CocosUIListener::anyKeyPressed(void) const
{
    // ��Cocos2d-x�У�ʹ�ô�����ť�¼�������Ҫ��������
    return false;
}

void CocosUIListener::onLibClosed(void)
{
    if (logLabel)
    {
        logLabel->setString("Library closed.");
    }
}
