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

//�Զ��忨������������
constexpr int EDIT_START_X = 200;       // ��ѡ��X����
constexpr int EDIT_START_Y = 200;       // ��ѡ��Y����
constexpr int EDIT_SELECT_WIDTH = 1000;    // ��ѡ�����
constexpr int EDIT_SELECT_HEIGHT = 1000;    // ��ѡ���߶�
constexpr int CARD_PER_ROW = 5;         // ��ѡ��ÿ�п�����
constexpr int EDIT_SELECTED_X = 1330;    // ��ѡ����X����
constexpr int EDIT_SELECTED_Y = 200;     // ��ѡ����Y����
constexpr int EDIT_SELECTED_WIDTH = 600;    // ��ѡ�������
constexpr int EDIT_SELECTED_HEIGHT = 1000;    // ��ѡ�����߶�

//����ѡ������������
constexpr float SELECT_ICON_SCALE = 0.8f;  // ͼ�����ű���
constexpr int SELECT_ICON_SPACING = 350;  // ͼ����
constexpr int SELECT_START_X = 300;    // ��һ��ͼ���X����
constexpr int SELECT_START_Y = 800;    // ��һ��ͼ���Y����
constexpr int SELECT_ICONS_PER_ROW = 3;     // ÿ��ͼ����
extern  int SELECT_CARDS;  // ѡ��Ķ�Ӧ�ƿ�
 
//MatchingScene ƥ�䳡������
constexpr int MATCHING_SCENE_CANCEL_OFFSET_X = 30;      // ƥ�䳡�� ȡ����ť X ƫ����
constexpr int MATCHING_SCENE_CANCEL_OFFSET_Y = -440;      // ƥ�䳡�� ȡ����ť Y ƫ�������������Ļ���ģ�
constexpr float MATCHING_SCENE_DURATION = 0.5f;              // ƥ��ҳ��ͣ��ʱ�������ԣ�����ʽ��

// BoardScene������������
// ��Ƭ�������ʼλ������
constexpr int CARD_REGION_X = 100;
constexpr int CARD_REGION_Y = 100;
constexpr int PUTOUT_CARD_REGION_HALF_X = 700;     // ����������X
constexpr int PUTOUT_CARD_REGION_HALF_Y = 300;     // ����������Y

// ���峣�������Ը���ʵ�����������
constexpr int PLAYED_AREA_PLAYER1_X = 400;   // ��������Ѵ��������ʾ�������ʼX����
constexpr int PLAYED_AREA_PLAYER2_X = 1300;  // �Է�����Ѵ��������ʾ�������ʼX����
constexpr int PLAYED_AREA_Y = 600;			// ������ʾ��Y����
constexpr int CARD_SPACING = 120;			// ����֮���ˮƽ���

// ���ó�������
constexpr int SETSCENE_SLIDER_X = 1050;                                // ���ó����������������ǩλ�� X 
constexpr int SETSCENE_BGMSLIDER_Y = 820;                              // ���ó������������������������ǩλ�� Y 
constexpr int SETSCENE_EFFSLIDER_Y = 650;                             // ���ó�����Ч�������������ǩλ�� Y 
constexpr int SETSCENE_RETURN_BUTTON_X = 1900;                        // ���ó������ز˵���ťλ�� X 
constexpr int SETSCENE_RETURN_BUTTON_Y = 50;                          // ���ó������ز˵���ťλ�� Y 
constexpr int SETSCENE_CLOSE_BUTTON_X = 850;                        // ���ó����˳���Ϸ��ťλ�� X 
constexpr int SETSCENE_CLOSE_BUTTON_Y = 470;                          // ���ó����˳���Ϸ��ťλ�� Y 

// �̵��ƶ����ã�Ԥ����

// λ����������Ļ�������ã�Ԥ����


// ս���������ã�Ԥ����

// ����뻶ӭ��ʾ���ֵ��

#endif // !_CONSTANT_H_