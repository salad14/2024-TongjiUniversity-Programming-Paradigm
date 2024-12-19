// test/main.cpp
#include <iostream>
#include <memory>

// 包含所有必要的头文件
#include "SpellManager.h"
#include "Manager.h"
#include "Spell.h"
#include "Minion.h"
#include "MinionCard.h"
#include <Windows.h>

int main() {
    // 设置控制台编码为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8); 

    // 创建 SpellManager 实例并加载 spells.json
    SpellManager spellManager;
    if (!spellManager.loadSpellsFromFile("spells.json")) {
        std::cerr << "Failed to load spells." << std::endl;
        return -1;
    }

    // 创建 Manager 实例
    Manager gameManager;

    // 创建一个 MinionCard 实例
    // 假设 MinionCard 有一个构造函数如下：
    // MinionCard(int dbfId, const std::string& name, int attack, int health)
    MinionCard minionCard(20001, "Test Minion", 5, 10); // 示例参数

    // 创建一个 Minion 实例，通过 MinionCard 初始化
    std::shared_ptr<Minion> minion = std::make_shared<Minion>(&minionCard);

    // 将 Minion 添加到 Manager 中
    // 使用 minionCard.dbfId 作为 ID，因为 Minion 类中没有 getId()
    gameManager.addMinion(minionCard.dbfId, minion);

    // 获取并播放火球术
    int fireballDbfId = 12345;
    auto fireballCard = spellManager.getSpellCardById(fireballDbfId);
    if (fireballCard) {
        // 创建 Spell 实例
        Spell fireball(fireballCard);
        std::cout << "Playing spell: " << fireball.getName() << " with cost " << fireball.getCost() << std::endl;

        // 修改费用，例如减少 1 点费用
        fireball.setCost(fireball.getCost() - 1);
        std::cout << "Modified cost: " << fireball.getCost() << std::endl;

        // 执行法术效果，目标是随从 20001
        std::vector<int> targets = { minionCard.dbfId };
        fireball.play(&gameManager, targets, minion.get());

        // 更新随从状态
        minion->updateStatus();
    }
    else {
        std::cerr << "Fireball card not found." << std::endl;
    }

    // 获取并播放寒冰箭
    int frostboltDbfId = 12346;
    auto frostboltCard = spellManager.getSpellCardById(frostboltDbfId);
    if (frostboltCard) {
        // 创建 Spell 实例
        Spell frostbolt(frostboltCard);
        std::cout << "Playing spell: " << frostbolt.getName() << " with cost " << frostbolt.getCost() << std::endl;

        // 不修改费用
        // 执行法术效果，目标是随从 20001
        std::vector<int> targets = { minionCard.dbfId };
        frostbolt.play(&gameManager, targets, minion.get());

        // 更新随从状态
        minion->updateStatus();
    }
    else {
        std::cerr << "Frostbolt card not found." << std::endl;
    }

    getchar();
    getchar();
    return 0;
}
