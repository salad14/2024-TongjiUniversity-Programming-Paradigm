/****************************************************************
 ��Ƶ���淽��
 ���ֵ������������StartScene.cpp�ļ��У�������������ֻ�����ô�ͷ�ļ���������audioPlayer()����
 �Ա�����Ϸ��ʼ����������������������ض������ �� ������ͳһ����
 ****************************************************************/
#ifndef _AUDIO_PLAYER_H_
#define _AUDIO_PLAYER_H_
#include "AudioEngine.h"
#include "cocos2d.h"
#include "proj.win32/Alluse.h"

 // ��Ƶ��������
extern int g_backgroundMusicSign;
extern int g_soundEffectSign;
extern float g_backgroundMusicVolumn;
extern float g_soundEffectVolumn;

// ��Ƶ���淽��
void audioPlayer(const std::string& audioPath, bool isLoop);

#endif // !_AUDIO_PLAYER_H_
