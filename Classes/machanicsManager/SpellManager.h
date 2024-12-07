#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include "card/SpellCard.h"
#include "card/MinionCard.h"

//�������и�minion�йصĲ���������
class Manager {
public:
	// ����Ŀ��
	void frozen(const std::vector<int>& targetList) {
		for (int targetId : targetList) {
			// ������һ��������ȡ��Ӷ���
			MinionCard* minion = getMinionById(targetId);
			if (minion) {
				//minion->setFrozen(true);
				// ����Ҫ����UI����Spell�/board���
			}
		}
	}

	void grow(MinionCard& target, int health, int attack, int crystal) {
		//target.setHealth(target.getHealth() + health);
		//target.setAttack(target.getAttack() + attack);
		// ����ˮ���仯�������ӷ��û�����Ч��
	}


	void crystal(int round,int crystal) {
		if (round == -1) {
			// �������
		}
		else if (round == 0) {
			// �����غ���ʱ����ˮ��
		}
		else if (round == 1) {
			// ������������ˮ��
		}
	}

	// ��Ŀ������˺�
	void damage(const std::vector<int>& targetList, int damage) {
		for (int targetId : targetList) {
			MinionCard* minion = getMinionById(targetId);
			if (minion) {
				//minion->setHealth(minion->getHealth() - damage);
				//if (minion->getHealth() <= 0) {
					// �����������
					//removeMinion(targetId);
				//}
				// ����Ҫ����UI
			}
		}
	}

	// �鿨
	void draw(int num) {
		for (int i = 0; i < num; ++i) {
			// ʵ�ֳ鿨�߼�������ӿ�����ȡһ�ſ��������� board��ʵ�֣�
			// updateUI();
		}
	}

	void registerSecret(int dbfid, std::function<void()> effect) {
		secretEffects[dbfid] = effect;
	}

	void triggerSecret(int dbfid) {
		auto it = secretEffects.find(dbfid);
		if (it != secretEffects.end()) {
			it->second();              // ִ�а���Ч��
			secretEffects.erase(it);   // ͨ���������Ƴ�
		}
		else {
			//Logger::error("No secret effect registered for card ID " + std::to_string(dbfId));
		}
	}

private:
	std::unordered_map<int, std::function<void()>> secretEffects;
	

	MinionCard* getMinionById(int id) {
		// ʵ�ֻ�ȡ��ӵ��߼�
		return nullptr; 
	}

	void removeMinion(int id) {
		// ʵ���Ƴ���ӵ��߼�
	}
}; 