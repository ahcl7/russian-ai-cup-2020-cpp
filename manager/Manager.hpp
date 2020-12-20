//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_MANAGER_HPP
#define AICUP_CPP_MANAGER_HPP

#include "builder/BuilderManager.hpp"
#include "attacker/AttackerManager.hpp"

class Manager {
public:
    PlayerView playerView;
    BuilderManager builderManager;
    AttackerManager attackerManager;
    Manager() {};
    Manager(const PlayerView& playerView1) {
        builderManager = BuilderManager(playerView1);
        attackerManager = AttackerManager(playerView1);
    }
    EntityAction getActionForBuilder(int entityId) { return builderManager.getAction(entityId);};
    EntityAction getActionForAttacker(int entityId) { return attackerManager.getAction(entityId);};
    void update(const PlayerView& playerView1) {
        this->playerView = playerView1;
        builderManager.update(playerView1);
    };

};
#endif //AICUP_CPP_MANAGER_HPP
