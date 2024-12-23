// Player.h
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <vector>
#include <string>
#include <utility>
#include "../card/ALLCard.h"

typedef int PlayerNumber; // 玩家编号，例如 1 或 2
typedef int CardNumber;   // 卡牌编号，每个玩家内部唯一

class GameData; // 前向声明

namespace players {

    class Player
    {
    public:
        // 构造函数，接受 GameData 的引用
        Player(PlayerNumber number, const std::string& nickname, GameData& gameData);

        // 添加卡牌到手牌
        // void addCardToHand(std::shared_ptr<CardBase> card);

        // 从手牌中移除卡牌
        void removeCardFromHand(CardNumber cardNumber);

        // 获取玩家昵称
        std::string getNickname() const;

        // 获取和设置生命值
        int getHealth() const;
        void setHealth(int hp);
        bool getDamage(int damage); // true 存活  false 死亡

        // 获取和设置法力值
        int getMoney() const; // 获取当前法力值
        void setMoney(int m); // 设置当前法力值
        int getMaxMoney() const; // 获取法力值上限
        void setMaxMoney(int maxm); //  设置最大法力值
        void increaseMana();     // 增加法力值

        // 玩家携带的卡组
        std::vector<CardNumber> getPlayerCards() const;

        // 初始化牌库
        void initializeDeck();

        // 抽牌相关
        std::shared_ptr<CardBase> drawCard(); // 返回 cardNumber
        bool hasCards() const;
        //void resetDeck();

        // 获取手牌
        // const std::vector<CardNumber>& getHand() const { return hand; }

        // 处理过度抽牌
        void handleOverdraw();

        // 获取当前过度抽牌次数
        int getOverdrawCount() const { return overdrawCount; }

        // 获取 playerNumber
        PlayerNumber getPlayerNumber() const { return playerNumber; }

        // 洗牌方法
        void shuffleDeck();

        // 析构函数
        ~Player();

        int health = 30;   // 当前生命值
        int money = 0;     // 当前法力值
        int roundmoney = 0; // 当前回合法力值
        int maxmoney = 10;  // 最大法力值
    private:
        GameData& gameData; // 引用 GameData

        PlayerNumber playerNumber; // 玩家编号

        int cardNumberCounter;

        std::string nickname;
        
        std::vector<CardNumber> playerCards;  // 玩家携带的卡组

        // 牌库管理
        // std::vector<CardNumber> deck; // 每张卡牌的 cardNumber
        //std::vector<CardNumber> hand; // 手牌

        std::vector<std::shared_ptr<CardBase>> hand; // 手牌
        std::vector<std::shared_ptr<CardBase>> newdeck; // 牌库

        // 过度抽牌计数器
        int overdrawCount = 0;
    };

} // namespace players

#endif // _PLAYER_H_
