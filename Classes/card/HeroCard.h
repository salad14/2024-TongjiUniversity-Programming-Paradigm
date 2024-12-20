#pragma once
#include <string>
#include <vector>
#include "MinionCard.h"
#include "cocos2d.h"

class HeroCard : MinionCard{
public:
	HeroCard() = default;

	// reset attack
	void resetAttack() { this->attack = 0; }

	void equipweapon(int attack, int durablity) {
		this->weaponAttack = attack;
		this->weaponDurability = durablity;
	}

	// ¼¼ÄÜ
	virtual void HeroPower() = 0;

public:
	// weapon??
	int weaponAttack = 0;
	int weaponDurability = 0;
};