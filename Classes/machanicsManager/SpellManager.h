#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include "card/SpellCard.h"
#include "card/MinionCard.h"

//以下所有跟minion有关的操作都待定
class Manager {
public:
	// 冻结目标
	void frozen(const std::vector<int>& targetList) {
		for (int targetId : targetList) {
			// 假设有一个方法获取随从对象
			MinionCard* minion = getMinionById(targetId);
			if (minion) {
				//minion->setFrozen(true);
				// 还需要更新UI（放Spell里？/board里？）
			}
		}
	}

	void grow(MinionCard& target, int health, int attack, int crystal) {
		//target.setHealth(target.getHealth() + health);
		//target.setAttack(target.getAttack() + attack);
		// 处理水晶变化，如增加费用或其他效果
	}


	void crystal(int round,int crystal) {
		if (round == -1) {
			// 处理过载
		}
		else if (round == 0) {
			// 处理本回合临时增加水晶
		}
		else if (round == 1) {
			// 处理永久增加水晶
		}
	}

	// 对目标造成伤害
	void damage(const std::vector<int>& targetList, int damage) {
		for (int targetId : targetList) {
			MinionCard* minion = getMinionById(targetId);
			if (minion) {
				//minion->setHealth(minion->getHealth() - damage);
				//if (minion->getHealth() <= 0) {
					// 处理随从死亡
					//removeMinion(targetId);
				//}
				// 还需要更新UI
			}
		}
	}

	// 抽卡
	void draw(int num) {
		for (int i = 0; i < num; ++i) {
			// 实现抽卡逻辑，例如从卡组中取一张卡加入手牌 board中实现？
			// updateUI();
		}
	}

	void registerSecret(int dbfid, std::function<void()> effect) {
		secretEffects[dbfid] = effect;
	}

	void triggerSecret(int dbfid) {
		auto it = secretEffects.find(dbfid);
		if (it != secretEffects.end()) {
			it->second();              // 执行奥秘效果
			secretEffects.erase(it);   // 通常触发后移除
		}
		else {
			//Logger::error("No secret effect registered for card ID " + std::to_string(dbfId));
		}
	}

private:
	std::unordered_map<int, std::function<void()>> secretEffects;
	

	MinionCard* getMinionById(int id) {
		// 实现获取随从的逻辑
		return nullptr; 
	}

	void removeMinion(int id) {
		// 实现移除随从的逻辑
	}
}; 