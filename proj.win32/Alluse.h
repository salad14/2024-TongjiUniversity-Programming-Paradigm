#pragma once
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
constexpr float STARTUP_SCENE_DURATION = 3.0f;                             // ������������ʱ��
constexpr float SCENE_TRANSITION_DURATION = 0.2f;                           // �����л�ʱ��

// ��ʼ����Helloworld����
constexpr int NICKNAME_MAX_LENGTH = 6;                                      // ��Ϸ�ǳ���󳤶�
constexpr int INITIAL_SCENE_FONT_SIZE = 30;                                 // ��ʼ���������С
constexpr int INITIAL_SCENE_LABELS_OFFSET_X = 349;                          // ��ʼ������ǩλ�� X ƫ����
constexpr int INITIAL_SCENE_TEXTFIELD_OFFSET_Y = 111;                       // ��ʼ�����ı���λ�� Y ƫ����
constexpr int INITIAL_SCENE_PROMPT_LABEL_OFFSET_Y = 180;                    // ��ʼ������ʾ��ǩλ�� Y ƫ����
constexpr int INITIAL_SCENE_NAME_LABEL_OFFSET_Y = 280;                      // ��ʼ������ӭ��ǩλ�� Y ƫ����
constexpr int INITIAL_SCENE_BUTTON_OFFSET_Y = -310;                         // ��ʼ������ťλ�� Y ƫ����
constexpr float PROMPT_MESSAGE_DURATION = 1.5f;                             // ��ʾ��Ϣͣ��ʱ��

// �˵���������


// ��ս�������ã�Ԥ����


// ���ó������ã�Ԥ����

// �̵��ƶ����ã�Ԥ����

// λ����������Ļ�������ã�Ԥ����
constexpr int WAITING_AREA_START_X = 415;                                   // ��ս����ʼ��Ļ����λ�� X ����
constexpr int WAITING_AREA_START_Y = 195;                                   // ��ս����ʼ��Ļ����λ�� Y ����
constexpr int BATTLE_AREA_START_X = 415;                                    // ս������ʼ��Ļ����λ�� X ����
constexpr int BATTLE_AREA_START_Y = 275;                                    // ս������ʼ��Ļ����λ�� Y ����
constexpr int CHAMPION_HORIZONTAL_INTERVAL = 60;                            // ս��Ӣ��ˮƽ���
constexpr int CHAMPION_VERTICAL_INTERVAL = 45;                              // ս��Ӣ�۴�ֱ���

// ս���������ã�Ԥ����

// ����뻶ӭ��ʾ���ֵ��

#endif // !_CONSTANT_H_