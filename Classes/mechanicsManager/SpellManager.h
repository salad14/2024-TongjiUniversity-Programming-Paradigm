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

    // ��ȡ����ʵ��
    static SpellManager& getInstance() {
        static SpellManager instance;
        return instance;
    }

    // ���ط�����������
    bool loadSpellsFromFile(const std::string& filepath);

    // ���� dbfId ��ȡ SpellCard
    std::shared_ptr<SpellCard> getSpellCardById(int dbfId) const;

    // ��ȡ���з�������
    const std::unordered_map<int, std::shared_ptr<SpellCard>>& getAllSpells() const;


private:
    std::unordered_map<int, std::shared_ptr<SpellCard>> spellCards;
};