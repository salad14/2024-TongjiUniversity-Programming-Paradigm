/****************************************************************
* ��Ϸ���ý���Ķ���
* Author: Lee
 ****************************************************************/
#include "cocos2d.h"


 /*
  * Class Name:     SettingsScene
  * Class Function: ���ó�����
  */
class SetScene : public cocos2d::Scene {
public:
    // ��������
    static cocos2d::Scene* createScene();

    // ��ʼ������
    virtual bool init();

    void cancelCallback(Ref* pSender);// ȡ����ť
    void SetScene::setCloseCallback(Ref* pSender);//�˳���Ϸ��ť

    // ʵ�� SettingsScene ��� create ����
    CREATE_FUNC(SetScene);
};