/****************************************************************
 ��Ϸȫ�ָ������ü���������
 ****************************************************************/
#ifndef _CONSTANT_H_
#define _CONSTANT_H_
#include <map>
#include <string>

 // Ӧ�ó�������
constexpr float FRAME_RATE = 60.0f;                                         // Ӧ�ó���֡��
const std::string APPLICATION_TITLE = u8"Hearthstone ¯ʯ��˵";             // Ӧ�ó������
// �����������

constexpr int DEFAULT_MUSIC_SIGN = -1;                                      // ȫ��ȱʡ���ֱ�ʶ
constexpr float DEFAULT_MUSIC_VOLUMN = 0.1f;                                // ȫ��ȱʡ������С

// ���绷������(Ԥ��)


// ��ɫ����
constexpr int DARK_BLUE_R = 0;                                              // ����ɫ R ͨ��
constexpr int DARK_BLUE_G = 32;                                             // ����ɫ G ͨ��
constexpr int DARK_BLUE_B = 96;                                             // ����ɫ B ͨ��
constexpr int GOLDEN_R = 223;                                               // ���ɫ R ͨ��
constexpr int GOLDEN_G = 176;                                               // ���ɫ G ͨ��
constexpr int GOLDEN_B = 11;                                                // ���ɫ B ͨ��

// ��������StartScene����
constexpr int STARTUP_SCENE_FONT_SIZE = 20;                                 // �������������С
constexpr int STARTUP_SCENE_LOADINGBAR_OFFSET_Y = -480;                     // �����������ؽ�����λ�� Y ƫ�������������Ļ���ģ�
constexpr int STARTUP_SCENE_LOADING_LABEL_OFFSET_X = -30;                   // �����������ؽ�������ǩλ�� X ƫ����
constexpr float STARTUP_SCENE_DURATION = 1.0f;                              // ������������ʱ��
constexpr float SCENE_TRANSITION_DURATION = 0.2f;                           // �����л�ʱ��

// ��ʼ����Helloworld����
constexpr float PROMPT_MESSAGE_DURATION = 1.5f;                             // ��ʾ��Ϣͣ��ʱ��

// MainScene�˵���������
constexpr int MAIN_SCENE_PLAYBUTTON_OFFSET_Y = 200;                  // MainScene PLAY ���� Y ƫ�������������Ļ���ģ�
constexpr int MAIN_SCENE_PLAYBUTTON_OFFSET_X = 0;                   // MainScene PLAY ���� X ƫ����
constexpr int MAIN_SCENE_ADVBUTTON_OFFSET_Y = 100;                    // MainScene adventure ���� Y ƫ�������������Ļ���ģ�
constexpr int MAIN_SCENE_ADVBUTTON_OFFSET_X = 0;                    // MainScene adventure ���� X ƫ����

//MatchingScene ƥ�䳡������
constexpr int MATCHING_SCENE_MATCHING_LABEL_OFFSET_Y = 0; // ƥ�䳡��ƥ���б�ǩλ�� Y ƫ�������������Ļ���ģ�
constexpr int MATCHING_SCENE_MATCHING_LABEL_OFFSET_X = 0;    // ƥ�䳡��ƥ���б�ǩλ�� X ƫ����
constexpr float MATCHING_SCENE_DURATION = 1.0f;              // ƥ��ҳ��ͣ��ʱ�������ԣ�����ʽ��

// ��ս�������ã�Ԥ����
// 
// ���ó������ã�Ԥ����

// �̵��ƶ����ã�Ԥ����

// λ����������Ļ�������ã�Ԥ����


// ս���������ã�Ԥ����

// ����뻶ӭ��ʾ���ֵ��

#endif // !_CONSTANT_H_