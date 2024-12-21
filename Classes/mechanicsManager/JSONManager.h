#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "../card/MinionCard.h"
#include "../card/SpellCard.h"
#include "iostream"

class JSONManager {
public:
	JSONManager(const std::string& fileName) {
		load_cards(fileName);
	}

    // ��ȡ JSON �ļ�������
    void load_cards(const std::string& fileName) {
        std::ifstream file(fileName);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + fileName);
            return;
        }
        json j;
        file >> j;
        cards.clear();
        for (const auto& item : j) {
            // ÿ�ſ�������
            cards.push_back(create_card_from_json(item));
            cards.push_back(create_card_from_json(item));
        }
    }

    // ������Ƭ���󣺸��� JSON ���ݴ�����Ӧ���������
    std::unique_ptr<CardBase> create_card_from_json(const json& j) {
        std::string cardType = j.value("type", "");

        // ���� "type" �ֶ��жϿ�Ƭ������
        if (cardType == "MINION") {
            auto card = std::make_unique<MinionCard>();
            card->from_json(j);
            return card;
        }
        else if (cardType == "SPELL") {
            auto card = std::make_unique<SpellCard>();
            card->from_json(j);
            // effects
            if (j.contains("effects") && j["effects"].is_array()) {
                for (const auto& effectJson : j["effects"]) {
                    std::string type = effectJson.value("type", "");
                    Effect effect;
                    if (type == "Damage") {
                        effect.type = SpellMechanics::KeyWord::Damage;
                        effect.amount = effectJson.value("amount", 0);
                        card->addMechanic(SpellMechanics::KeyWord::Damage);
                    }
                    else if (type == "Draw") {
                        effect.type = SpellMechanics::KeyWord::Draw;
                        effect.amount = effectJson.value("amount", 0);
                        card->addMechanic(SpellMechanics::KeyWord::Draw);
                    }
                    else if (type == "Frozen") {
                        effect.type = SpellMechanics::KeyWord::Frozen;
                        effect.amount = 0;
                        card->addMechanic(SpellMechanics::KeyWord::Frozen);
                    }

                    card->addEffect(effect);
                }
            }
            return card;
        }
        else if (cardType == "WEAPON") {}
        //else if (cardType == "HERO") {}
        else {
            throw std::runtime_error("Unknown card type: " + cardType);
            return nullptr;
        }
    }

public:
    void getdeck(std::vector<std::shared_ptr<CardBase>>& newdeck) {
        std::vector<std::shared_ptr<CardBase>> selectedCards;
        for (auto i : cards) {
            newdeck.push_back(i);  // ֱ�Ӹ��� shared_ptr
        }
    }

    // ����ӿ�
    // �� dbfId ���ҿ�Ƭ
    std::shared_ptr<CardBase> find_by_dbfId(int dbfId) const {
        for (const auto& card : cards) {
            if (card->dbfId == dbfId) {
                return card;
            }
        }
        return nullptr;
    }

    int getCardCost(int dbfId) const{
        for (const auto& card : cards) {
            if (card->dbfId == dbfId) {
                return card->cost;
            }
        }
        throw std::runtime_error("Failed to get card cost");
        return -1;
    }

    // �� cardClass ���ҿ�Ƭ ��ʱû��ʹ��
    std::vector<std::shared_ptr<CardBase>> find_by_cardClass(const cardClass cardClass) const {
        std::vector<std::shared_ptr<CardBase>> result;
        for (const auto& card : cards) {
            if (card->cardClass == cardClass) {
                result.push_back(card);
            }
        }
        return result;
    }

    // ͨ�ò�ѯ����  ������������ɸѡ
    template <typename Predicate>
    std::vector<std::shared_ptr<CardBase>> find_cards(Predicate pred) const {
        vector<std::shared_ptr<CardBase>> result;
        for (const auto& card : cards) {
            if (pred(card)) {
                result.push_back(card);
            }
        }
        return result;
    }

    // ������п�Ƭ��Ϣ  �������
    void print_all_cards() const {
        for (const auto& card : cards) {
            std::cout << "dbfId: " << card->dbfId << ", cardClass: " << card->cardClass << ", name: " << card->name << std::endl;
            throw  std::runtime_error("Failed to print cards");
        }
    }

private:
    std::vector<std::shared_ptr<CardBase>> cards;  // ʹ�� shared_ptr �������ڴ�
};