#include "BoardScene.h"

USING_NS_CC;

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

    return true;
}