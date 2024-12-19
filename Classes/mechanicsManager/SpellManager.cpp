// classes/mechanicsManager/SpellManager.cpp
#include "SpellManager.h"
#include <fstream>
#include <iostream>

bool SpellManager::loadSpellsFromFile(const std::string& filepath) {
    using json = nlohmann::json;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open JSON file: " << filepath << std::endl;
        return false;
    }

    json j;
    try {
        file >> j;
    }
    catch (json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    }

    if (!j.contains("cards") || !j["cards"].is_array()) {
        std::cerr << "Invalid JSON format: 'cards' array not found." << std::endl;
        return false;
    }

    for (const auto& cardJson : j["cards"]) {
        int dbfId = cardJson.value("dbfId", 0);
        std::string name = cardJson.value("name", "Unknown");
        int cost = cardJson.value("cost", 0);
        std::string cardClassStr = cardJson.value("cardClass", "NEUTRAL");
        std::string text = cardJson.value("text", "");
        std::string typeStr = cardJson.value("type", "SPELL");
        std::string rarityStr = cardJson.value("rarity", "COMMON");
        std::string spellSchoolStr = cardJson.value("spellSchool", "FIRE");

        // 解析 cardClass
        cardClass cClass = cardClass::NEUTRAL;
        if (cardClassStr == "MAGE") cClass = cardClass::MAGE;
        else if (cardClassStr == "WARRIOR") cClass = cardClass::WARRIOR;
        else if (cardClassStr == "DRUID") cClass = cardClass::DRUID;
        else if (cardClassStr == "ROGUE") cClass = cardClass::ROGUE;
        // 默认 NEUTRAL

        // 解析 rarity
        cardRarity rarity = cardRarity::COMMON;
        if (rarityStr == "COMMON") rarity = cardRarity::COMMON;
        else if (rarityStr == "LEGENDARY") rarity = cardRarity::LEGENDARY;


        // 解析 spellSchool
        SpellSchool school = SpellSchool::FIRE;
        if (spellSchoolStr == "FIRE") school = SpellSchool::FIRE;
        else if (spellSchoolStr == "FROST") school = SpellSchool::FROST;
        else if (spellSchoolStr == "ARCANE") school = SpellSchool::ARCANE;


        // 创建 SpellCard 对象
        auto spellCard = std::make_shared<SpellCard>(dbfId, name, cost, cClass, text, rarity, school);

        // 解析 effects
        if (cardJson.contains("effects") && cardJson["effects"].is_array()) {
            for (const auto& effectJson : cardJson["effects"]) {
                std::string type = effectJson.value("type", "");
                Effect effect;
                if (type == "Damage") {
                    effect.type = SpellMechanics::KeyWord::Damage;
                    effect.amount = effectJson.value("amount", 0);
                    spellCard->addMechanic(SpellMechanics::KeyWord::Damage);
                }
                else if (type == "Draw") {
                    effect.type = SpellMechanics::KeyWord::Draw;
                    effect.amount = effectJson.value("amount", 0);
                    spellCard->addMechanic(SpellMechanics::KeyWord::Draw);
                }
                else if (type == "Frozen") {
                    effect.type = SpellMechanics::KeyWord::Frozen;
                    effect.amount = 0;
                    spellCard->addMechanic(SpellMechanics::KeyWord::Frozen);
                }
                else if (type == "Grow") {
                    effect.type = SpellMechanics::KeyWord::Grow;
                    effect.amount = effectJson.value("amount", 0);
                    spellCard->addMechanic(SpellMechanics::KeyWord::Grow);
                }
                else if (type == "Crystal") {
                    effect.type = SpellMechanics::KeyWord::Crystal;
                    effect.amount = effectJson.value("amount", 0);
                    spellCard->addMechanic(SpellMechanics::KeyWord::Crystal);
                }

                spellCard->addEffect(effect);
            }
        }

        // 存储到 spellCards 容器中
        spellCards[dbfId] = spellCard;
    }

    std::cout << "Loaded " << spellCards.size() << " spell cards from " << filepath << std::endl;
    return true;
}

std::shared_ptr<SpellCard> SpellManager::getSpellCardById(int dbfId) const {
    auto it = spellCards.find(dbfId);
    if (it != spellCards.end()) {
        return it->second;
    }
    return nullptr;
}
