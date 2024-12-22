// Player.cpp
#include "Player.h"
#include "GameData.h"
#include "cocos2d.h"
#include "proj.win32/Alluse.h"

namespace players {

    Player::Player(PlayerNumber number, const std::string& nickname, GameData& gameData)
        : playerNumber(number), nickname(nickname), health(30), money(0), maxmoney(10), overdrawCount(0), gameData(gameData)
    {
        CCLOG("Creating Player %d: %s", playerNumber, nickname.c_str());
        initializeDeck();
        if (playerNumber == 1) {
            cardNumberCounter = 1000; // Player1 �� cardNumber ��1000��ʼ
        }
        else if (playerNumber == 2) {
            cardNumberCounter = 2000; // Player2 �� cardNumber ��2000��ʼ
        }
    }

    // ��ӿ��Ƶ�����
    //void Player::addCardToHand(std::shared_ptr<CardBase> card)
    //{
    //    
    //}

    // ���������Ƴ�����
    void Player::removeCardFromHand(CardNumber cardNumber)
    {
        for (auto it = hand.begin(); it != hand.end(); ++it) {
            if (it->get()->dbfId == cardNumber) {
                hand.erase(it);
            }
        }
    }

    // ��ȡ����ǳ�
    std::string Player::getNickname() const
    {
        return nickname;
    }

    // ��ȡ����������ֵ
    int Player::getHealth() const
    {
        return health;
    }

    void Player::setHealth(int hp)
    {
        health = hp;
        CCLOG("Player %d: %s, Health set to: %d", playerNumber, nickname.c_str(), health);
    }

    bool Player::getDamage(int damage) {
        health -= damage;
        if (health <= 0) return false;
        else return true;
    }

    void Player::increaseMana() {
        if (money < maxmoney) {
            money += 1;
        }
    }


    // ��ȡ�����÷���ֵ
    int Player::getMoney() const
    {
        return money;
    }

    void Player::setMoney(int m)
    {
        money = m;
        CCLOG("Player %d: %s, Money set to: %d", playerNumber, nickname.c_str(), money);
    }

    int Player::getMaxMoney() const
    {
        return maxmoney;
    }

    void Player::setMaxMoney(int maxm)
    {
        maxmoney = maxm;
        CCLOG("Player %d: %s, MaxMoney set to: %d", playerNumber, nickname.c_str(), maxmoney);
    }

    // ��ȡ���Я���Ŀ���
    std::vector<CardNumber> Player::getPlayerCards() const
    {
        return playerCards;
    }

    // ��ʼ���ƿ�
    void Player::initializeDeck() {
        if (SELECT_CARDS == 1) {
            JSONManager manager("cards/json/cards1.json");
            manager.getdeck(newdeck);
        }
        else if (SELECT_CARDS == 2) {
            JSONManager manager("cards/json/cards2.json");
            manager.getdeck(newdeck);
        }
        
        CCLOG("Initializing deck for player %d: %s", playerNumber, nickname.c_str());

        //// ʾ���ֶ���ӿ��� (�������ÿ�������20�ſ���)
        //for (int i = 0; i < 20; ++i) {
        //    CardNumber cardNum = gameData.getUniqueCardNumber(); // ʹ�ô��ݵ� gameData ���û�ȡΨһ���Ʊ��
        //    playerCards.push_back(cardNum);
        //    deck.emplace_back(cardNum);
        //    CCLOG("Player %d: Added cardNumber: %d to deck: %s", playerNumber, cardNum, nickname.c_str());
        //}

        // ϴ��
        shuffleDeck();
        // CCLOG("Player %d: Deck shuffled. Total cards in deck: %zu", playerNumber, deck.size());
    }

    // ϴ�Ʒ���
    void Player::shuffleDeck() {
        //if (deck.empty()) {
        //    CCLOG("Player %d: Cannot shuffle. Deck is empty: %s", playerNumber, nickname.c_str());
        //    return;
        //}

        //std::random_device rd;
        //std::mt19937 g(rd());
        //std::shuffle(deck.begin(), deck.end(), g);
        //CCLOG("Player %d: Shuffled deck: %s", playerNumber, nickname.c_str());

        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(newdeck.begin(), newdeck.end(), g);
    }

    // ���ƿ��г�ȡһ�ſ���
    std::shared_ptr<CardBase> Player::drawCard()
    {
        if (newdeck.empty()) {
            CCLOG("Player %d: Deck is empty. Attempting to reset deck: %s", playerNumber, nickname.c_str());
            //resetDeck();
            handleOverdraw();

            return NULL;
        }
        
        //CardNumber cardNumber = newdeck.back();
        //newdeck.pop_back();

        // ��ӵ�����
        std::shared_ptr<CardBase> card = newdeck.back();
        // addCardToHand(card);
        hand.push_back(card);
        newdeck.pop_back();

        CCLOG("Player %d: %s drew cardNumber: %d from deck. Remaining deck size: %zu",
            playerNumber, nickname.c_str(), card->dbfId, newdeck.size());

        return card;
    }

    // �������Ƿ��п��ƿ��Գ�
    bool Player::hasCards() const
    {
        return !newdeck.empty();
    }

    // �����ƿ�
    //void Player::resetDeck()
    //{
    //    CCLOG("Player %d: Resetting deck: %s", playerNumber, nickname.c_str());
    //    // �������������¼��뵽�ƿ�
    //    for (auto& cardNum : hand) {
    //        deck.emplace_back(cardNum);
    //        CCLOG("Player %d: Returned cardNumber: %d to deck: %s", playerNumber, cardNum, nickname.c_str());
    //    }
    //    hand.clear();

    //    // ϴ��
    //    shuffleDeck();
    //    CCLOG("Player %d: Deck reset and shuffled. Total cards in deck: %zu", playerNumber, deck.size());
    //}

    // ������ȳ��ƣ���Ѫ��
    void Player::handleOverdraw()
    {
        overdrawCount++;
        health -= overdrawCount;
        CCLOG("Player %d: %s has overdrawn. Health reduced by %d. Current health: %d",
            playerNumber, nickname.c_str(), overdrawCount, health);
    }

    // ��������
    Player::~Player()
    {
        CCLOG("Destroying Player %d: %s", playerNumber, nickname.c_str());
        hand.clear();
    }

} // namespace players
