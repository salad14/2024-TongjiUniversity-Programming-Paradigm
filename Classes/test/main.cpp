#include "Card.h"

int main() {

    // 注册战吼和亡语效果
    manager.registerBattlecry(1, []() {
        std::cout << "Battlecry: Deal 3 damage to the enemy hero!" << std::endl;
        });

    manager.registerBattlecry(2, []() {
        std::cout << "Battlecry: Draw a card!" << std::endl;
        });

    manager.registerDeathrattle(1, []() {
        std::cout << "Deathrattle: Summon a 1/1 token!" << std::endl;
        });

    manager.registerDeathrattle(2, []() {
        std::cout << "Deathrattle: Gain 3 Armor!" << std::endl;
        });

    // 创建卡牌对象
    MinionCard* minion1 = new MinionCard(1, "Fire Elemental", 5, 6, 5);
    MinionCard* minion2 = new MinionCard(2, "Arcane Intellect", 2, 2, 3);

    minion1->mechanics.push_back(keyWord::Battlecry);
    minion1->mechanics.push_back(keyWord::Deathrattle);
    minion2->mechanics.push_back(keyWord::Battlecry);
    minion2->mechanics.push_back(keyWord::Deathrattle);

    // 执行卡牌的战吼和亡语
    minion1->play();  // 输出：Battlecry: Deal 3 damage to the enemy hero!
    minion1->die();   // 输出：Deathrattle: Summon a 1/1 token!

    minion2->play();  // 输出：Battlecry: Draw a card!
    minion2->die();   // 输出：Deathrattle: Gain 3 Armor!

    delete minion1;
    delete minion2;

    return 0;
}
