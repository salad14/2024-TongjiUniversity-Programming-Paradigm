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
        cardClass cClass = NEUTRAL;
        if (cardClassStr == "MAGE") cClass = MAGE;
        else if (cardClassStr == "WARRIOR") cClass = WARRIOR;
        else if (cardClassStr == "DRUID") cClass = DRUID;
        else if (cardClassStr == "ROGUE") cClass = ROGUE;
        // 默认 NEUTRAL

        // 解析 rarity
        cardRarity rarity = COMMON;
        if (rarityStr == "COMMON") rarity = COMMON;
        else if (rarityStr == "LEGENDARY") rarity = LEGENDARY;
        // 其他 rarity 可扩展

        // 解析 spellSchool
        SpellSchool school = SpellSchool::FIRE;
        if (spellSchoolStr == "FIRE") school = SpellSchool::FIRE;
        else if (spellSchoolStr == "FROST") school = SpellSchool::FROST;
        else if (spellSchoolStr == "ARCANE") school = SpellSchool::ARCANE;
        // 其他 spellSchool 可扩展

        // 创建 SpellCard 对象
        auto spellCard = std::make_shared<SpellCard>(dbfId, name, cost, cClass, text, rarity, school);

        // 解析 effects
        if (cardJson.contains("effects") && cardJson["effects"].is_array()) {
            for (const auto& effectJson : cardJson["effects"]) {
                std::string type = effectJson.value("type", "");
                Effect effect;
                if (type == "Damage") {
                    effect.type = Spell::KeyWord::Damage;
                    effect.amount = effectJson.value("amount", 0);
                    spellCard->addMechanic(Spell::KeyWord::Damage);
                }
                else if (type == "Draw") {
                    effect.type = Spell::KeyWord::Draw;
                    effect.amount = effectJson.value("amount", 0);
                    spellCard->addMechanic(Spell::KeyWord::Draw);
                }
                else if (type == "Frozen") {
                    effect.type = Spell::KeyWord::Frozen;
                    effect.amount = 0;
                    spellCard->addMechanic(Spell::KeyWord::Frozen);
                }
                else if (type == "Grow") {
                    effect.type = Spell::KeyWord::Grow;
                    effect.amount = effectJson.value("amount", 0);
                    spellCard->addMechanic(Spell::KeyWord::Grow);
                }
                else if (type == "Crystal") {
                    effect.type = Spell::KeyWord::Crystal;
                    effect.amount = effectJson.value("amount", 0);
                    spellCard->addMechanic(Spell::KeyWord::Crystal);
                }
                // 其他 effect 类型可扩展

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
