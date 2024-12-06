#ifndef MANAGER
#define MANAGER

#include <unordered_map>
#include <functional>
#include <string>
#include "Card.h"

class keyWordManager {
private:
    // �洢����ID��ս������Ч��֮���ӳ���ϵ
    std::unordered_map<int, std::function<void()>> battlecryEffects;
    std::unordered_map<int, std::function<void()>> deathrattleEffects;

public:
    // ע��ս��Ч��
    void registerBattlecry(int dbfid, std::function<void()> effect) {
        battlecryEffects[dbfid] = effect;
    }

    // ע������Ч��
    void registerDeathrattle(int dbfid, std::function<void()> effect) {
        deathrattleEffects[dbfid] = effect;
    }

    // �����ض�ID��ս��Ч��
    void triggerBattlecry(int dbfid) {
        if (battlecryEffects.find(dbfid) != battlecryEffects.end()) {
            battlecryEffects[dbfid]();
        }
        else {
            throw std::runtime_error("No battlecry effect registered for card ID " + std::to_string(dbfid));
        }
    }

    // �����ض�ID������Ч��
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