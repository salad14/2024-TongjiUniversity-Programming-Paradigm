/****************************************************************
 音频引擎方法的定义
 ****************************************************************/
#ifndef _AUDIO_PLAYER_H_
#define _AUDIO_PLAYER_H_
#include "AudioEngine.h"
#include "cocos2d.h"
#include "proj.win32/Alluse.h"

 // 音频引擎设置
int backgroundMusicSign = DEFAULT_MUSIC_SIGN;
int soundEffectSign = DEFAULT_MUSIC_SIGN;
float backgroundMusicVolumn = DEFAULT_MUSIC_VOLUMN;
float soundEffectVolumn = DEFAULT_MUSIC_VOLUMN;

// 音频引擎方法
void audioPlayer(const std::string& audioPath, bool isLoop)
{
    if (isLoop) {
        if (backgroundMusicSign != DEFAULT_MUSIC_SIGN) {
            cocos2d::experimental::AudioEngine::stop(backgroundMusicSign);
        }
        backgroundMusicSign = cocos2d::experimental::AudioEngine::play2d(audioPath, isLoop);
        cocos2d::experimental::AudioEngine::setVolume(backgroundMusicSign, backgroundMusicVolumn);
    }
    else {
        soundEffectSign = cocos2d::experimental::AudioEngine::play2d(audioPath, isLoop);
        cocos2d::experimental::AudioEngine::setVolume(soundEffectSign, soundEffectVolumn);
    }
}

#endif // !_AUDIO_PLAYER_H_
