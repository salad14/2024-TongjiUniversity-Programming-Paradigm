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
    // ��Դ��Cocos2d-x�Զ�����
}

// CocosUIListener.cpp

#include "CocosUIListener.h"
#include "cocos2d.h"

void CocosUIListener::initializeLogLabel(cocos2d::Layer* layer, const cocos2d::Vec2& position)
{
    // ��ȡ�ɼ��ߴ����趨��ǩ���
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

    // �������и��������С������30���ı�ǩ�����ù̶���ȣ��Զ��߶�
    logLabel = cocos2d::Label::createWithSystemFont(
        "",                  // ��ʼ�ı�Ϊ��
        "Arial",             // ����
        30,                  // �����С
        cocos2d::Size(visibleSize.width - 40, 0), // �̶���ȣ��Զ��߶�
        cocos2d::TextHAlignment::CENTER,        // ˮƽ���ж���
        cocos2d::TextVAlignment::TOP            // ��ֱ���˶���
    );

    if (logLabel == nullptr)
    {
        CCLOG("Failed to create logLabel.");
        return;
    }

    // ���ñ�ǩλ��
    logLabel->setPosition(position);

    // ������ɫ���������ǿ�ɶ���
    logLabel->setColor(cocos2d::Color3B::WHITE);
    logLabel->enableOutline(cocos2d::Color4B::BLACK, 2); // ��ɫ��ߣ���߿��Ϊ2

    // ���ñ�ǩ���뷽ʽ
    logLabel->setAlignment(cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::TOP);

    // ��ӱ�ǩ��ָ���Ĳ��У��㼶Ϊ10
    layer->addChild(logLabel, 10);
}

void CocosUIListener::writeString(const ExitGames::Common::JString& str)
{
    if (logLabel)
    {
        cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([this, str]() {
            // �� JString ת��Ϊ std::wstring
            std::wstring wstr = str.cstr();

            // �� std::wstring ת��Ϊ UTF-8 std::string
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::string utf8Str = converter.to_bytes(wstr);

            logLabel->setString(utf8Str);
            });
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
