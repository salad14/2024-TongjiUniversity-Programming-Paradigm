// Player.h
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <vector>
#include <string>
#include <utility>
#include "../card/ALLCard.h"

typedef int PlayerNumber; // ��ұ�ţ����� 1 �� 2
typedef int CardNumber;   // ���Ʊ�ţ�ÿ������ڲ�Ψһ

class GameData; // ǰ������

namespace players {

    class Player
    {
    public:
        // ���캯�������� GameData ������
        Player(PlayerNumber number, const std::string& nickname, GameData& gameData);

        // ��ӿ��Ƶ�����
        // void addCardToHand(std::shared_ptr<CardBase> card);

        // ���������Ƴ�����
        void removeCardFromHand(CardNumber cardNumber);

        // ��ȡ����ǳ�
        std::string getNickname() const;

        // ��ȡ����������ֵ
        int getHealth() const;
        void setHealth(int hp);
        bool getDamage(int damage); // true ���  false ����

        // ��ȡ�����÷���ֵ
        int getMoney() const; // ��ȡ��ǰ����ֵ
        void setMoney(int m); // ���õ�ǰ����ֵ
        int getMaxMoney() const; // ��ȡ����ֵ����
        void setMaxMoney(int maxm); //  ���������ֵ
        void increaseMana();     // ���ӷ���ֵ

        // ���Я���Ŀ���
        std::vector<CardNumber> getPlayerCards() const;

        // ��ʼ���ƿ�
        void initializeDeck();

        // �������
        std::shared_ptr<CardBase> drawCard(); // ���� cardNumber
        bool hasCards() const;
        //void resetDeck();

        // ��ȡ����
        // const std::vector<CardNumber>& getHand() const { return hand; }

        // ������ȳ���
        void handleOverdraw();

        // ��ȡ��ǰ���ȳ��ƴ���
        int getOverdrawCount() const { return overdrawCount; }

        // ��ȡ playerNumber
        PlayerNumber getPlayerNumber() const { return playerNumber; }

        // ϴ�Ʒ���
        void shuffleDeck();

        // ��������
        ~Player();

        int health = 30;   // ��ǰ����ֵ
        int money = 0;     // ��ǰ����ֵ
        int roundmoney = 0; // ��ǰ�غϷ���ֵ
        int maxmoney = 10;  // �����ֵ
    private:
        GameData& gameData; // ���� GameData

        PlayerNumber playerNumber; // ��ұ��

        int cardNumberCounter;

        std::string nickname;
        
        std::vector<CardNumber> playerCards;  // ���Я���Ŀ���

        // �ƿ����
        // std::vector<CardNumber> deck; // ÿ�ſ��Ƶ� cardNumber
        //std::vector<CardNumber> hand; // ����

        std::vector<std::shared_ptr<CardBase>> hand; // ����
        std::vector<std::shared_ptr<CardBase>> newdeck; // �ƿ�

        // ���ȳ��Ƽ�����
        int overdrawCount = 0;
    };

} // namespace players

#endif // _PLAYER_H_
