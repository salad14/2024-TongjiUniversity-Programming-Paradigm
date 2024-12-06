#ifndef MANAGER
#define MANAGER

#include <unordered_map>
#include <functional>
#include <string>
#include "Card.h"

class keyWordManager {
private:
    // 存储卡牌ID与战吼、亡语效果之间的映射关系
    std::unordered_map<int, std::function<void()>> battlecryEffects;
    std::unordered_map<int, std::function<void()>> deathrattleEffects;

public:
    // 注册战吼效果
    void registerBattlecry(int dbfid, std::function<void()> effect) {
        battlecryEffects[dbfid] = effect;
    }

    // 注册亡语效果
    void registerDeathrattle(int dbfid, std::function<void()> effect) {
        deathrattleEffects[dbfid] = effect;
    }

    // 调用特定ID的战吼效果
    void triggerBattlecry(int dbfid) {
        if (battlecryEffects.find(dbfid) != battlecryEffects.end()) {
            battlecryEffects[dbfid]();
        }
        else {
            throw std::runtime_error("No battlecry effect registered for card ID " + std::to_string(dbfid));
        }
    }

    // 调用特定ID的亡语效果
    void triggerDeathrattle(int dbfid) {
        if (deathrattleEffects.find(dbfid) != deathrattleEffects.end()) {
            deathrattleEffects[dbfid]();
        }
        else {
            throw std::runtime_error("No deathrattle effect registered for card ID " + std::to_string(dbfid));
        }
    }
};

keyWordManager manager;

#endif