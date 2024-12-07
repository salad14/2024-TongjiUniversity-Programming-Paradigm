/****************************************************************
 游戏全局各类设置及常量定义
 ****************************************************************/
#ifndef _CONSTANT_H_
#define _CONSTANT_H_
#include <map>
#include <string>

 // 应用程序设置
constexpr float FRAME_RATE = 60.0f;                                         // 应用程序帧率
const std::string APPLICATION_TITLE = u8"Hearthstone 炉石传说";             // 应用程序标题
// 音乐相关设置

constexpr int DEFAULT_MUSIC_SIGN = -1;                                      // 全局缺省音乐标识
constexpr float DEFAULT_MUSIC_VOLUMN = 0.1f;                                // 全局缺省音量大小

// 网络环境设置(预留)


// 颜色设置
constexpr int DARK_BLUE_R = 0;                                              // 深蓝色 R 通道
constexpr int DARK_BLUE_G = 32;                                             // 深蓝色 G 通道
constexpr int DARK_BLUE_B = 96;                                             // 深蓝色 B 通道
constexpr int GOLDEN_R = 223;                                               // 金黄色 R 通道
constexpr int GOLDEN_G = 176;                                               // 金黄色 G 通道
constexpr int GOLDEN_B = 11;                                                // 金黄色 B 通道

// 启动场景StartScene设置
constexpr int STARTUP_SCENE_FONT_SIZE = 20;                                 // 启动场景字体大小
constexpr int STARTUP_SCENE_LOADINGBAR_OFFSET_Y = -480;                     // 启动场景加载进度条位置 Y 偏移量（相对于屏幕中心）
constexpr int STARTUP_SCENE_LOADING_LABEL_OFFSET_X = -30;                   // 启动场景加载进度条标签位置 X 偏移量
constexpr float STARTUP_SCENE_DURATION = 1.0f;                              // 启动场景加载时间
constexpr float SCENE_TRANSITION_DURATION = 0.2f;                           // 场景切换时间

// 初始场景Helloworld设置
constexpr float PROMPT_MESSAGE_DURATION = 1.5f;                             // 提示信息停留时间

// MainScene菜单场景设置
constexpr int MAIN_SCENE_PLAYBUTTON_OFFSET_Y = 200;                  // MainScene PLAY 按键 Y 偏移量（相对于屏幕中心）
constexpr int MAIN_SCENE_PLAYBUTTON_OFFSET_X = 0;                   // MainScene PLAY 按键 X 偏移量
constexpr int MAIN_SCENE_ADVBUTTON_OFFSET_Y = 100;                    // MainScene adventure 按键 Y 偏移量（相对于屏幕中心）
constexpr int MAIN_SCENE_ADVBUTTON_OFFSET_X = 0;                    // MainScene adventure 按键 X 偏移量

//MatchingScene 匹配场景设置
constexpr int MATCHING_SCENE_MATCHING_LABEL_OFFSET_Y = 0; // 匹配场景匹配中标签位置 Y 偏移量（相对于屏幕中心）
constexpr int MATCHING_SCENE_MATCHING_LABEL_OFFSET_X = 0;    // 匹配场景匹配中标签位置 X 偏移量
constexpr float MATCHING_SCENE_DURATION = 1.0f;              // 匹配页面停留时长（测试，非正式）

// 对战场景设置（预留）
// 
// 设置场景设置（预留）

// 商店牌堆设置（预留）

// 位置属性与屏幕坐标设置（预留）


// 战斗参数设置（预留）

// 序号与欢迎提示语键值对

#endif // !_CONSTANT_H_