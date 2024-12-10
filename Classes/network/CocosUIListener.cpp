// CocosUIListener.cpp
#include "CocosUIListener.h"
#include "cocos2d.h"
#include <codecvt>
#include <locale>

USING_NS_CC;

// ��ȡ Singleton ʵ��
CocosUIListener* CocosUIListener::getInstance()
{
    static CocosUIListener instance;
    return &instance;
}

// ˽�й��캯��
CocosUIListener::CocosUIListener()
    : logLabel(nullptr)
{
    // ��ѡ������־�ļ�
    logFileStream.open("game_log.txt", std::ios::app);
    if (!logFileStream.is_open())
    {
        CCLOG("Failed to open log file.");
    }
}

// ˽����������
CocosUIListener::~CocosUIListener()
{
    if (logFileStream.is_open())
    {
        logFileStream.close();
    }
}

void CocosUIListener::attachToLayer(cocos2d::Layer* layer, const cocos2d::Vec2& position)
{
    // ������� logLabel���ȴ�֮ǰ�� Layer ���Ƴ�
    if (logLabel && logLabel->getParent())
    {
        logLabel->removeFromParent();
    }

    // �����µ� logLabel
    logLabel = cocos2d::Label::createWithSystemFont(
        "",                  // ��ʼ�ı�Ϊ��
        "Arial",             // ����
        30,                  // �����С
        cocos2d::Size(layer->getContentSize().width - 40, 0), // �̶���ȣ��Զ��߶�
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

    // ��ѡ����¼��־���ļ�
    if (logFileStream.is_open())
    {
        logFileStream << "Log label attached to new layer." << std::endl;
    }
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

            // ���� logLabel ���ı�
            logLabel->setString(utf8Str);

            // ��ѡ����¼��־���ļ�
            if (logFileStream.is_open())
            {
                logFileStream << utf8Str << std::endl;
            }
            });
    }
    else
    {
        CCLOG("CocosUIListener::writeString called but logLabel is null.");
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

        // ��ѡ����¼��־���ļ�
        if (logFileStream.is_open())
        {
            logFileStream << "Library closed." << std::endl;
        }
    }
}
