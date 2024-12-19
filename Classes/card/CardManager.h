#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "card.h"
#include "SpellCard.h"
#include "../external/json/json.hpp"

class CardManager {
public:
    // Load card data from a JSON file
    bool loadCardsFromFile(const std::string& filePath);

    // Get a card by its dbfId
    std::shared_ptr<CardBase> getCardById(int dbfId) const;

private:
    std::unordered_map<int, std::shared_ptr<CardBase>> cards;

    // Get the cardClass enum from a string
    cardClass getCardClassFromString(const std::string& classStr) const;

    // Get the cardRarity enum from a string
    cardRarity getCardRarityFromString(const std::string& rarityStr) const;

    // Get the SpellSchool enum from a string
    SpellSchool getSpellSchoolFromString(const std::string& schoolStr) const;

    // Get the cardType enum from a string
    cardType getCardTypeFromString(const std::string& typeStr) const;
};
#pragma once
