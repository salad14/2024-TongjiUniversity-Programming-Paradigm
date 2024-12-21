// Classes/mechanicsManager/Spelleffects

#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>
#include "../entity/Minion.h"


// ???§à??Žï????????????TO DO?I
class SpellEffects {
public:
    // ???????

    void frozen(Minion* minion) {
        minion->setFrozen(true);
    }

    // ???????????
    void damage(Minion* minion, int damage) {
        minion->takeDamage(damage);
    }

    // ??
    void draw(int num) {
        //TO DO std::cout << "Drawing " << num << " card(s)." << std::endl;
    }

};