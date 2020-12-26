//
// Created by Apple on 14/11/2020.
//

#include "InformationForBaseManager.hpp"

InformationForBaseManager::InformationForBaseManager(const PlayerView &playerView) {
    this->update(playerView);
}

void InformationForBaseManager::update(const PlayerView &playerView) {
    numberOfAttackers = 0;
    numberOfBuilders = 0;
    for(int i = 0 ; i < playerView.entities.size(); i++) {
        const Entity& entity = playerView.entities[i];
        EntityType entityType = entity.entityType;
        if (Utils::isMyEntity(entity)) {
            if (entityType == BUILDER_UNIT) {
                numberOfBuilders ++;
            }
            if (entityType == RANGED_UNIT || entityType == MELEE_UNIT) {
                numberOfAttackers ++;
            }
//            if (entityType == RANGED_BASE || entityType == BUILDER_BASE) {
//                bases.push_back(entity);
//                if (entityType == RANGED_BASE) numberOfRangedBases ++;
//            }
        }
    }
}