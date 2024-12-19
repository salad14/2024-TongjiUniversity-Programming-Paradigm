#include "CardManager.h"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

bool CardManager::loadCardsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filePath << std::endl;
        return false;
    }

    json j;
    try {
        file >> j;
    }
    catch (json::parse_error& e) {
        std::cerr << "JSON 解析错误: " << e.what() << std::endl;
        return false;
    }

    for (const auto& cardJson : j["cards"]) {
        int dbfId = cardJson["dbfId"];
        std::string name = cardJson["name"];
        int cost = cardJson["cost"];
        std::string classStr = cardJson["cardClass"];
        std::string text = cardJson["text"];
        std::string typeStr = cardJson["type"];
        std::string rarityStr = cardJson["rarity"];
        std::string schoolStr = cardJson.value("spellSchool", ""); 

        // static_cast!
        cardType cardTypeEnum = static_cast<cardType>(getCardTypeFromString(typeStr));
        cardClass cardClassEnum = static_cast<cardClass>(getCardClassFromString(classStr));
        cardRarity cardRarityEnum = static_cast<cardRarity>(getCardRarityFromString(rarityStr));

        if (cardTypeEnum == SPELL) {
            SpellSchool spellSchoolEnum = schoolStr.empty() ? SpellSchool::ARCANE : getSpellSchoolFromString(schoolStr);
            std::shared_ptr<SpellCard> spellCard = std::make_shared<SpellCard>(
                dbfId, name, cost, cardClassEnum, text, cardRarityEnum, spellSchoolEnum
            );

            // parse effects and add to SpellCard
            if (cardJson.contains("effects")) {
                for (const auto& effectJson : cardJson["effects"]) {
                    std::string effectType = effectJson["type"];
                    if (effectType == "Damage") {
                        int amount = effectJson["amount"];
                        spellCard->addMechanic(Spell::Damage);
                        spellCard->setDamageAmount(amount);
                    }
                    else if (effectType == "Draw") {
                        int amount = effectJson.value("amount", 1);
                        spellCard->addMechanic(Spell::Draw);
                        spellCard->setDrawAmount(amount);
                    }
                    else if (effectType == "Frozen") {
                        spellCard->addMechanic(Spell::Frozen);
                    }
                }
            }


            cards[dbfId] = spellCard;
        }

    }

    return true;
}

std::shared_ptr<CardBase> CardManager::getCardById(int dbfId) const {
    auto it = cards.find(dbfId);
    if (it != cards.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<CardBase>> CardManager::getAllCards() const {
    std::vector<std::shared_ptr<CardBase>> allCards;
    for (const auto& pair : cards) {
        allCards.emplace_back(pair.second);
    }
    return allCards;
}

cardClass CardManager::getCardClassFromString(const std::string& classStr) const {
    if (classStr == "MAGE") return MAGE;
    if (classStr == "WARRIOR") return WARRIOR;
    if (classStr == "DRUID") return DRUID;
    if (classStr == "ROGUE") return ROGUE;
    return NEUTRAL;
}

cardRarity CardManager::getCardRarityFromString(const std::string& rarityStr) const {
    if (rarityStr == "COMMON") return COMMON;
    if (rarityStr == "LEGENDARY") return LEGENDARY;
    return COMMON;
}

SpellSchool CardManager::getSpellSchoolFromString(const std::string& schoolStr) const {
    if (schoolStr == "FIRE") return SpellSchool::FIRE;
    if (schoolStr == "FROST") return SpellSchool::FROST;
    if (schoolStr == "ARCANE") return SpellSchool::ARCANE;

    return SpellSchool::ARCANE;
}

cardType CardManager::getCardTypeFromString(const std::string& typeStr) const {
    if (typeStr == "SPELL") return SPELL;
    if (typeStr == "MINION") return MINION;
    if (typeStr == "WEAPON") return WEAPON;
    if (typeStr == "HERO") return HERO;
    return MINION;
}
