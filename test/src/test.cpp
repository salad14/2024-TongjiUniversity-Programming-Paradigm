/*该文件是测试集成json库的文件，最终选用的是nlohmann这个库，
因为非常简单无需向其他库一样有繁琐的包含配置，只需要包含json.hpp一个文件即可,
使用方法可以参照本文件和下面的blog https://www.stubbornhuang.com/2731/
vscode中编译命令为g++ -std=c++17 -Iinclude src/test.cpp -o test.exe
.\test.exe > output.txt(防止编码格式不正确)*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <locale>
#include <filesystem>

// json 库
#include "../include/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

enum cardClass {
    MAGE,
    WARRIOR,
    DRUID,
    ROGUE,
    NEUTRAL
};

enum cardType {
    SPELL,
    MINION,
    WEAPON,
    HERO
};

enum cardRarity {
    COMMON,
    LEGENDARY
};

enum SpellSchool {
    FIRE,
    FROST,
    ARCANE
};

enum SpellEffectType {
    DAMAGE,
    DRAW,
    FROZEN
};

class SpellCard {
public:
    int dbfId;
    std::string name;
    int cost;
    cardClass classType;
    std::string text;
    cardType type;
    cardRarity rarity;
    SpellSchool spellSchool;
    std::vector<std::pair<SpellEffectType, int>> effects; // 第二个元素用于存储数值，如 Damage 的 amount

    void printInfo() const {
        std::cout << "SpellCard ID: " << dbfId << std::endl;
        std::cout << "Name: " << name << std::endl;
        std::cout << "Cost: " << cost << std::endl;
        std::cout << "Class: ";
        switch (classType) {
            case MAGE: std::cout << "MAGE"; break;
            case WARRIOR: std::cout << "WARRIOR"; break;
            case DRUID: std::cout << "DRUID"; break;
            case ROGUE: std::cout << "ROGUE"; break;
            case NEUTRAL: std::cout << "NEUTRAL"; break;
            default: std::cout << "UNKNOWN"; break;
        }
        std::cout << std::endl;

        std::cout << "Rarity: ";
        switch (rarity) {
            case COMMON: std::cout << "COMMON"; break;
            case LEGENDARY: std::cout << "LEGENDARY"; break;
            default: std::cout << "UNKNOWN"; break;
        }
        std::cout << std::endl;

        std::cout << "Spell School: ";
        switch (spellSchool) {
            case FIRE: std::cout << "FIRE"; break;
            case FROST: std::cout << "FROST"; break;
            case ARCANE: std::cout << "ARCANE"; break;
            default: std::cout << "UNKNOWN"; break;
        }
        std::cout << std::endl;

        std::cout << "Effects: ";
        for (const auto& effect : effects) {
            switch (effect.first) {
                case DAMAGE:
                    std::cout << "Damage(" << effect.second << ") ";
                    break;
                case DRAW:
                    std::cout << "Draw(" << effect.second << ") ";
                    break;
                case FROZEN:
                    std::cout << "Frozen ";
                    break;
                default:
                    std::cout << "UNKNOWN ";
                    break;
            }
        }
        std::cout << std::endl;

        std::cout << "Description: " << text << std::endl;
        std::cout << "-----------------------------------" << std::endl;
    }
};

cardClass getCardClassFromString(const std::string& classStr) {
    if (classStr == "MAGE") return MAGE;
    if (classStr == "WARRIOR") return WARRIOR;
    if (classStr == "DRUID") return DRUID;
    if (classStr == "ROGUE") return ROGUE;
    return NEUTRAL;
}

cardType getCardTypeFromString(const std::string& typeStr) {
    if (typeStr == "SPELL") return SPELL;
    if (typeStr == "MINION") return MINION;
    if (typeStr == "WEAPON") return WEAPON;
    if (typeStr == "HERO") return HERO;
    return SPELL; 
}

cardRarity getCardRarityFromString(const std::string& rarityStr) {
    if (rarityStr == "COMMON") return COMMON;
    if (rarityStr == "LEGENDARY") return LEGENDARY;
    return COMMON; 
}

SpellSchool getSpellSchoolFromString(const std::string& schoolStr) {
    if (schoolStr == "FIRE") return FIRE;
    if (schoolStr == "FROST") return FROST;
    if (schoolStr == "ARCANE") return ARCANE;
    return ARCANE; 
}

SpellEffectType getSpellEffectTypeFromString(const std::string& effectStr) {
    if (effectStr == "Damage") return DAMAGE;
    if (effectStr == "Draw") return DRAW;
    if (effectStr == "Frozen") return FROZEN;
    return DAMAGE;
}

int main() {

    // 以下打印工作目录的部分都是我调试用的
    std::cout << "当前工作目录: " << fs::current_path() << std::endl;

    // 列出当前目录下的所有文件
    std::cout << "当前目录下的文件列表：" << std::endl;
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        std::cout << entry.path().filename().string() << std::endl;
    }
    std::cout << "-----------------------------------" << std::endl;

    // 打开 JSON 文件
    std::ifstream file("cards.json");
    if (!file.is_open()) {
        std::cerr << "无法打开文件: cards.json" << std::endl;
        return -1;
    }

    // 解析 JSON
    json j;
    try {
        file >> j;
    } catch (json::parse_error& e) {
        std::cerr << "JSON 解析错误: " << e.what() << std::endl;
        return -1;
    }

    // 遍历卡牌
    for (const auto& cardJson : j["cards"]) {
        SpellCard card;
        card.dbfId = cardJson["dbfId"];
        card.name = cardJson["name"];
        card.cost = cardJson["cost"];
        card.classType = getCardClassFromString(cardJson["cardClass"]);
        card.text = cardJson["text"];
        card.type = getCardTypeFromString(cardJson["type"]);
        card.rarity = getCardRarityFromString(cardJson["rarity"]);
        card.spellSchool = getSpellSchoolFromString(cardJson["spellSchool"]);

        // 解析效果
        if (cardJson.contains("effects")) {
            for (const auto& effectJson : cardJson["effects"]) {
                std::string effectTypeStr = effectJson["type"];
                SpellEffectType effectType = getSpellEffectTypeFromString(effectTypeStr);
                int amount = effectJson.value("amount", 0);
                card.effects.emplace_back(effectType, amount);
            }
        }

        // 打印卡牌信息
        card.printInfo();
    }

    return 0;
}
