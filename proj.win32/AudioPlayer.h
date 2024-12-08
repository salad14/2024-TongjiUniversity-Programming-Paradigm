/****************************************************************
 音频引擎方法
 其初值及函数定义在StartScene.cpp文件中，在其他场景中只需引用此头文件，并调用audioPlayer()即可
 以便在游戏开始设置音量，避免后续出现重定义错误 及 音量不统一问题
 ****************************************************************/
#ifndef _AUDIO_PLAYER_H_
#define _AUDIO_PLAYER_H_
#include "AudioEngine.h"
#include "cocos2d.h"
#include "proj.win32/Alluse.h"

 // 音频引擎设置
extern int g_backgroundMusicSign;
extern int g_soundEffectSign;
extern float g_backgroundMusicVolumn;
extern float g_soundEffectVolumn;

// 音频引擎方法
void audioPlayer(const std::string& audioPath, bool isLoop);

#endif // !_AUDIO_PLAYER_H_
