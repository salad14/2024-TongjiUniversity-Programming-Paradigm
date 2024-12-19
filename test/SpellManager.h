// test/SpellManager.h
#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include "SpellCard.h"
#include "json.hpp"

class SpellManager {
public:
    SpellManager() = default;
    ~SpellManager() = default;

    bool loadSpellsFromFile(const std::string& filepath);
    std::shared_ptr<SpellCard> getSpellCardById(int dbfId) const;

private:
    std::unordered_map<int, std::shared_ptr<SpellCard>> spellCards;
};
