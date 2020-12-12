//
// Created by Apple on 14/11/2020.
//

#include "EntityGroups.hpp"

void EntityGroups::update(const PlayerView& playerView) {
    int myId = playerView.myId;
    for(int i = 0; i < playerView.entities.size(); i++) {
        const Entity& entity = playerView.entities[i];
        if (entity.playerId == nullptr || *entity.playerId != myId) continue;
        if (entity.entityType == BUILDER_UNIT) {
            builders.push_back(entity);
        }
        if (entity.entityType == RANGED_UNIT) {
            rangedUnits.push_back(entity);
        }
        if (entity.entityType == MELEE_UNIT) {
            meleeUnits.push_back(entity);
        }
    }
}