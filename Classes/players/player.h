// Player.h
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <vector>
#include <string>
#include <utility>

typedef int PlayerNumber; // ��ұ�ţ����� 1 �� 2
typedef int CardNumber;   // ���Ʊ�ţ�ÿ������ڲ�Ψһ

class GameData; // ǰ������

namespace players {

    class Player
    {
    public:
        // ���캯�������� GameData ������
        Player(PlayerNumber number, const std::string& nickname, GameData& gameData);

        // ���ӿ��Ƶ�����
        void addCardToHand(CardNumber cardNumber);

        // ���������Ƴ�����
        void removeCardFromHand(CardNumber cardNumber);

        // ��ȡ����ǳ�
        std::string getNickname() const;

        // ��ȡ����������ֵ
        int getHealth() const;
        void setHealth(int hp);

        // ��ȡ�����÷���ֵ
        int getMoney() const;
        void setMoney(int m);
        int getMaxMoney() const;
        void setMaxMoney(int maxm);

        // ���Я���Ŀ���
        std::vector<CardNumber> getPlayerCards() const;

        // ��ʼ���ƿ�
        void initializeDeck();

        // �������
        CardNumber drawCard(); // ���� cardNumber
        bool hasCards() const;
        void resetDeck();

        // ��ȡ����
        const std::vector<CardNumber>& getHand() const { return hand; }

        // �������ȳ���
        void handleOverdraw();

        // ��ȡ��ǰ���ȳ��ƴ���
        int getOverdrawCount() const { return overdrawCount; }

        // ��ȡ playerNumber
        PlayerNumber getPlayerNumber() const { return playerNumber; }

        // ϴ�Ʒ���
        void shuffleDeck();

        // ��������
        ~Player();

    private:
        GameData& gameData; // ���� GameData

        PlayerNumber playerNumber; // ��ұ��

        int cardNumberCounter;

        std::string nickname;
        int health = 30;   // ��ǰ����ֵ
        int money = 0;     // ��ǰ����ֵ
        int maxmoney = 1;  // �����ֵ
        std::vector<CardNumber> playerCards;  // ���Я���Ŀ���

        // �ƿ����
        std::vector<CardNumber> deck; // ÿ�ſ��Ƶ� cardNumber
        std::vector<CardNumber> hand; // ����

        // ���ȳ��Ƽ�����
        int overdrawCount = 0;
    };

} // namespace players

#endif // _PLAYER_H_
