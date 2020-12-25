//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_MANAGER_HPP
#define AICUP_CPP_MANAGER_HPP

#include "builder/BuilderManager.hpp"
#include "attacker/AttackerManager.hpp"
#include "base/BaseManager.hpp"

class Manager {
public:
    PlayerView playerView;
    BuilderManager builderManager;
    AttackerManager attackerManager;
    BaseManager baseManager;
    Manager() {};
    Manager(const PlayerView& playerView1) {
        builderManager = BuilderManager(playerView1);
        attackerManager = AttackerManager(playerView1);
        baseManager = BaseManager(playerView1);
    }
    EntityAction getActionForBuilder(int entityId) { return builderManager.getAction(entityId);};
    EntityAction getActionForAttacker(int entityId) { return attackerManager.getAction(entityId);};
    EntityAction getActionForBase(const Entity& entity) { return baseManager.getAction(entity, builderManager.infoFBM.needRangedBase);};
    void update(const PlayerView& playerView1) {
        this->playerView = playerView1;
        builderManager.update(playerView1);
        attackerManager.update(playerView1);
        baseManager.update(playerView1);
    };

};
#endif //AICUP_CPP_MANAGER_HPP
