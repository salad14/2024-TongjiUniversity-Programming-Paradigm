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

//自定义卡组界面相关设置
constexpr int EDIT_START_X = 200;       // 待选区X坐标
constexpr int EDIT_START_Y = 200;       // 待选区Y坐标
constexpr int EDIT_SELECT_WIDTH = 1000;    // 待选区宽度
constexpr int EDIT_SELECT_HEIGHT = 1000;    // 待选区高度
constexpr int CARD_PER_ROW = 5;         // 待选区每行卡牌数
constexpr int EDIT_SELECTED_X = 1330;    // 已选择区X坐标
constexpr int EDIT_SELECTED_Y = 200;     // 已选择区Y坐标
constexpr int EDIT_SELECTED_WIDTH = 600;    // 已选择区宽度
constexpr int EDIT_SELECTED_HEIGHT = 1000;    // 已选择区高度

//卡组选择界面相关设置
constexpr float SELECT_ICON_SCALE = 0.8f;  // 图标缩放比例
constexpr int SELECT_ICON_SPACING = 350;  // 图标间距
constexpr int SELECT_START_X = 300;    // 第一个图标的X坐标
constexpr int SELECT_START_Y = 800;    // 第一行图标的Y坐标
constexpr int SELECT_ICONS_PER_ROW = 3;     // 每行图标数
extern  int SELECT_CARDS;  // 选择的对应牌库
 
//MatchingScene 匹配场景设置
constexpr int MATCHING_SCENE_CANCEL_OFFSET_X = 30;      // 匹配场景 取消按钮 X 偏移量
constexpr int MATCHING_SCENE_CANCEL_OFFSET_Y = -440;      // 匹配场景 取消按钮 Y 偏移量（相对于屏幕中心）
constexpr float MATCHING_SCENE_DURATION = 0.5f;              // 匹配页面停留时长（测试，非正式）

// BoardScene牌桌场景设置
// 卡片区域的起始位置坐标
constexpr int CARD_REGION_X = 100;
constexpr int CARD_REGION_Y = 100;
constexpr int PUTOUT_CARD_REGION_HALF_X = 700;     // 出牌区域半宽X
constexpr int PUTOUT_CARD_REGION_HALF_Y = 300;     // 出牌区域半高Y

// 定义常量（可以根据实际需求调整）
constexpr int PLAYED_AREA_PLAYER1_X = 400;   // 本地玩家已打出卡牌显示区域的起始X坐标
constexpr int PLAYED_AREA_PLAYER2_X = 1300;  // 对方玩家已打出卡牌显示区域的起始X坐标
constexpr int PLAYED_AREA_Y = 600;			// 卡牌显示的Y坐标
constexpr int CARD_SPACING = 120;			// 卡牌之间的水平间距

// 设置场景设置
constexpr int SETSCENE_SLIDER_X = 1050;                                // 设置场景音量滑动条与标签位置 X 
constexpr int SETSCENE_BGMSLIDER_Y = 820;                              // 设置场景背景音乐音量滑动条与标签位置 Y 
constexpr int SETSCENE_EFFSLIDER_Y = 650;                             // 设置场景音效音量滑动条与标签位置 Y 
constexpr int SETSCENE_RETURN_BUTTON_X = 1900;                        // 设置场景返回菜单按钮位置 X 
constexpr int SETSCENE_RETURN_BUTTON_Y = 50;                          // 设置场景返回菜单按钮位置 Y 
constexpr int SETSCENE_CLOSE_BUTTON_X = 850;                        // 设置场景退出游戏按钮位置 X 
constexpr int SETSCENE_CLOSE_BUTTON_Y = 470;                          // 设置场景退出游戏按钮位置 Y 

// 商店牌堆设置（预留）

// 位置属性与屏幕坐标设置（预留）


// 战斗参数设置（预留）

// 序号与欢迎提示语键值对

#endif // !_CONSTANT_H_