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
            cardNumberCounter = 1000; // Player1 的 cardNumber 从1000开始
        }
        else if (playerNumber == 2) {
            cardNumberCounter = 2000; // Player2 的 cardNumber 从2000开始
        }
    }

    // 添加卡牌到手牌
    //void Player::addCardToHand(std::shared_ptr<CardBase> card)
    //{
    //    
    //}

    // 从手牌中移除卡牌
    void Player::removeCardFromHand(CardNumber cardNumber)
    {
        for (auto it = hand.begin(); it != hand.end(); ++it) {
            if (it->get()->dbfId == cardNumber) {
                hand.erase(it);
            }
        }
    }

    // 获取玩家昵称
    std::string Player::getNickname() const
    {
        return nickname;
    }

    // 获取和设置生命值
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


    // 获取和设置法力值
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

    // 获取玩家携带的卡组
    std::vector<CardNumber> Player::getPlayerCards() const
    {
        return playerCards;
    }

    // 初始化牌库
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

        //// 示例手动添加卡牌 (这里假设每个玩家有20张卡牌)
        //for (int i = 0; i < 20; ++i) {
        //    CardNumber cardNum = gameData.getUniqueCardNumber(); // 使用传递的 gameData 引用获取唯一卡牌编号
        //    playerCards.push_back(cardNum);
        //    deck.emplace_back(cardNum);
        //    CCLOG("Player %d: Added cardNumber: %d to deck: %s", playerNumber, cardNum, nickname.c_str());
        //}

        // 洗牌
        shuffleDeck();
        // CCLOG("Player %d: Deck shuffled. Total cards in deck: %zu", playerNumber, deck.size());
    }

    // 洗牌方法
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

    // 从牌库中抽取一张卡牌
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

        // 添加到手牌
        std::shared_ptr<CardBase> card = newdeck.back();
        // addCardToHand(card);
        hand.push_back(card);
        newdeck.pop_back();

        CCLOG("Player %d: %s drew cardNumber: %d from deck. Remaining deck size: %zu",
            playerNumber, nickname.c_str(), card->dbfId, newdeck.size());

        return card;
    }

    // 检查玩家是否还有卡牌可以抽
    bool Player::hasCards() const
    {
        return !newdeck.empty();
    }

    // 重置牌库
    //void Player::resetDeck()
    //{
    //    CCLOG("Player %d: Resetting deck: %s", playerNumber, nickname.c_str());
    //    // 将所有手牌重新加入到牌库
    //    for (auto& cardNum : hand) {
    //        deck.emplace_back(cardNum);
    //        CCLOG("Player %d: Returned cardNumber: %d to deck: %s", playerNumber, cardNum, nickname.c_str());
    //    }
    //    hand.clear();

    //    // 洗牌
    //    shuffleDeck();
    //    CCLOG("Player %d: Deck reset and shuffled. Total cards in deck: %zu", playerNumber, deck.size());
    //}

    // 处理过度抽牌（扣血）
    void Player::handleOverdraw()
    {
        overdrawCount++;
        health -= overdrawCount;
        CCLOG("Player %d: %s has overdrawn. Health reduced by %d. Current health: %d",
            playerNumber, nickname.c_str(), overdrawCount, health);
    }

    // 析构函数
    Player::~Player()
    {
        CCLOG("Destroying Player %d: %s", playerNumber, nickname.c_str());
        hand.clear();
    }

} // namespace players
