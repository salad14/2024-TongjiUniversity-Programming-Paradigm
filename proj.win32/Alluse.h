#pragma once
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
constexpr float STARTUP_SCENE_DURATION = 3.0f;                             // 启动场景加载时间
constexpr float SCENE_TRANSITION_DURATION = 0.2f;                           // 场景切换时间

// 初始场景Helloworld设置
constexpr int NICKNAME_MAX_LENGTH = 6;                                      // 游戏昵称最大长度
constexpr int INITIAL_SCENE_FONT_SIZE = 30;                                 // 初始场景字体大小
constexpr int INITIAL_SCENE_LABELS_OFFSET_X = 349;                          // 初始场景标签位置 X 偏移量
constexpr int INITIAL_SCENE_TEXTFIELD_OFFSET_Y = 111;                       // 初始场景文本框位置 Y 偏移量
constexpr int INITIAL_SCENE_PROMPT_LABEL_OFFSET_Y = 180;                    // 初始场景提示标签位置 Y 偏移量
constexpr int INITIAL_SCENE_NAME_LABEL_OFFSET_Y = 280;                      // 初始场景欢迎标签位置 Y 偏移量
constexpr int INITIAL_SCENE_BUTTON_OFFSET_Y = -310;                         // 初始场景按钮位置 Y 偏移量
constexpr float PROMPT_MESSAGE_DURATION = 1.5f;                             // 提示信息停留时间

// 菜单场景设置


// 对战场景设置（预留）


// 设置场景设置（预留）

// 商店牌堆设置（预留）

// 位置属性与屏幕坐标设置（预留）
constexpr int WAITING_AREA_START_X = 415;                                   // 候战区起始屏幕坐标位置 X 坐标
constexpr int WAITING_AREA_START_Y = 195;                                   // 候战区起始屏幕坐标位置 Y 坐标
constexpr int BATTLE_AREA_START_X = 415;                                    // 战斗区起始屏幕坐标位置 X 坐标
constexpr int BATTLE_AREA_START_Y = 275;                                    // 战斗区起始屏幕坐标位置 Y 坐标
constexpr int CHAMPION_HORIZONTAL_INTERVAL = 60;                            // 战斗英雄水平间距
constexpr int CHAMPION_VERTICAL_INTERVAL = 45;                              // 战斗英雄垂直间距

// 战斗参数设置（预留）

// 序号与欢迎提示语键值对

#endif // !_CONSTANT_H_