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

    bool loadSpellsFromFile(const std::string& filepath);
    std::shared_ptr<SpellCard> getSpellCardById(int dbfId) const;
    //shared_ptrÊÇÖÇÄÜÖ¸Õë¹ş
private:
    std::unordered_map<int, std::shared_ptr<SpellCard>> spellCards;
};
