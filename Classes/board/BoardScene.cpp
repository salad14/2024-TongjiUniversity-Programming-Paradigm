#include "BoardScene.h"
#include <vector>

using namespace std;
USING_NS_CC;

// ��Ƭ�������ʼλ������ ע�������þ���ê��Ϊ���½�
#define CARD_REGION_X 100
#define CARD_REGION_Y 20


Scene* BoardScene::createScene() {
    return BoardScene::create();
}

static void problemLoading(const char* filename) {
    printf("error, couldn't load file: %s\n", filename);
}

bool BoardScene::init() {
    if (!Scene::init()) {
        return false;
    }

    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto title_sprite = Sprite::create("board.png");
    if (title_sprite == nullptr) {
        problemLoading("board.png");
        return false;
    }
    else {
        // ����ͼƬ�ĳߴ�Ϊ���ڵĳߴ�
        title_sprite->setContentSize(visibleSize);
        // ��ͼƬ��λ�õ���Ϊ���ڵ�����
        title_sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        this->addChild(title_sprite, 0);
    }

    createCards();
    
    return true;
}

bool BoardScene::createCards()
{
    for (int i = 0; i < 5; i++)
    {
        auto card = Sprite::create("cardfortest.png");
        if (card == nullptr)
        {
            problemLoading("cardfortest.png");
            return false;
        }

        card->setPosition(Vec2(CARD_REGION_X + i * 120, CARD_REGION_Y));
        card->setScale(1.0f);
        card->setTag(i);
        card->setAnchorPoint(Vec2(0, 0));
        cards.push_back(card);
        this->addChild(card, 0);

        // ��������¼�������
        auto listener = EventListenerMouse::create();
        listener->onMouseDown = [this, card, i](Event* event) {
            onMouseDown(event, i);
        };
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, cards.at(i));
    }

    return true;
}

bool BoardScene::onMouseDown(Event* event, int cardIndex)
{
    auto mouseEvent = static_cast<EventMouse*>(event);

    // ������ʱ���߼�
    if (mouseEvent->getMouseButton() == cocos2d::EventMouse::MouseButton::BUTTON_LEFT)
    {
        //auto card = cards[cardIndex]; // ��ȡ��ǰ������Ŀ�Ƭ
        auto card = cards.at(cardIndex); // ��ȡ��ǰ������Ŀ�Ƭ
        if (card == nullptr)
        {
            problemLoading("cardfortest.png");
            return false;
        }

        // �����ǰ��Ƭ�Ѿ���ѡ�У�ȡ��ѡ��
        if (selectedCard == card)
        {
            selectedCard->setScale(1.0f); // �ָ�ԭ��С
            selectedCard = nullptr; // ȡ��ѡ��
        }
        else
        {
            // ������п�Ƭ��ѡ�У�ȡ��ѡ��
            if (selectedCard)
                selectedCard->setScale(1.0f); // �ָ�ԭ��С

            // ѡ�е�ǰ����Ŀ�Ƭ
            card->setScale(1.5f); // �Ŵ�ѡ�е���
            selectedCard = card;  // ��¼ѡ�е���
        }
    }
}