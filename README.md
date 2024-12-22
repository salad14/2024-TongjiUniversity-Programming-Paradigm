# Hearthstone

**Programming Paradigms course project, Tongji University 2024.**

Developed by students from software engineering, Computer Science and Technology.

**Engine:** Cocos2d-x 3.17.2 请确保版本一致

## 开发规范

1. 鉴于我们 ci 配置不成功，所有 pr 的 merge 只能人工进行（github 会检查基本合并冲突），请在提交 pr 前在本地先将 main 拉下来 merge，确保构建成功后再 push 并提 pr。
2. 合并审查只要大家看到了都可以审查
3. 自己分支上开发 -- 拉取 main 检查无误后提交 -- 提 pr 审 merge
4. 其他可能可选规范；resource 的统一命名格式（eg：HelloWorld.png），repo 和各分支的命名（如 repo 命名 hearthstone，各分支分别为 network/core-logic/ui-ux）...
5. 由于环境一样，.gitignore 文件中可以包括.vs/ Debug.win32/ release/ build/
6. 其他可能规范

## 版本注意事项：
12.06 大幅修改了文件架构，素材、头文件及场景文件都用文件夹重新组织，注意改动代码内的文件路径

12.07 重要头文件补充说明：proj.win32/Alluse.h内以常量形式放置了游戏全局需要的设置参数（如音量、颜色、位置坐标等）类似常量可以考虑放置在该头文件内； proj.win32\AudioPlayer.h 内统一设置了音频播放的方法。

12.08 关于AudioPlayer.h头文件的特别说明：其初值及函数定义在StartScene.cpp文件中，在其他场景中只需引用此头文件，并调用audioPlayer()即可。以便在游戏开始设置音量，避免后续出现重定义错误 及 音量不统一问题

12.09 鉴于目前尚未通过Photon中国的开发许可，匹配功能暂时不可用，基本逻辑已经预留，对于对战逻辑的相关代码尚未编写

12.19 在main分支初步实现了设置界面；设置可由主界面进入，目前可实现实时音量调节、退出游戏，后续可在SetScene.cpp中实现更多设置功能
      担心设置界面跳转会影响棋盘界面的实时联机匹配，因此暂时未将设置按钮添加到BoardScene.cpp中

12.20 初步实现了卡组选择界面（假设有6套初始卡组可选）
      注：目前的卡组选择界面暂时放在collection按钮下，与匹配界面衔接待完善

12.22 实现了人机对战模式的全流程，整合了现有的所有场景，现在主菜单所有按钮都有用：
      常规联机绑定在play按钮
      线下人机对战绑定在adventure按钮
      自定义卡组界面（仅有界面，底层逻辑待完善）绑定在collection按钮；
