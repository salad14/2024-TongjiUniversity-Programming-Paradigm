// classes/mechanicsManager/SpellManager.h
#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include "../card/SpellCard.h"
#include "external/json/json.hpp"

class SpellManager {
public:
    SpellManager() = default;
    ~SpellManager() = default;
    SpellManager(const SpellManager&) = delete;
    SpellManager& operator=(const SpellManager&) = delete;

    // 获取单例实例
    static SpellManager& getInstance() {
        static SpellManager instance;
        return instance;
    }

    // 加载法术卡牌数据
    bool loadSpellsFromFile(const std::string& filepath);

    // 根据 dbfId 获取 SpellCard
    std::shared_ptr<SpellCard> getSpellCardById(int dbfId) const;

    // 获取所有法术卡牌
    const std::unordered_map<int, std::shared_ptr<SpellCard>>& getAllSpells() const;


private:
    std::unordered_map<int, std::shared_ptr<SpellCard>> spellCards;
};