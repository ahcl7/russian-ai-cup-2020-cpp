//
// Created by Apple on 14/11/2020.
//
#include "InformationForBuilderManager.hpp"


InformationForBuilderManager::InformationForBuilderManager(const PlayerView& playerView) {
    currentResource = 0;
    currentPopulation = 0;
    providedPopulation = 0;
    numberOfBuilderBase = 0;
    numberOfMeleeBase = 0;
    numberOfRangedBase = 0;
    curTick = 0;
    bitmap = BitMap(playerView.mapSize);
};

bool InformationForBuilderManager::isInactiveEntity(int entityId) {
    for(int i = 0 ; i < inactiveEntities.size(); i++) {
        if (inactiveEntities[i].id == entityId) return true;
    }
    return false;
}

void InformationForBuilderManager::updateBuilders(const PlayerView& playerView) {
    map<int, BuilderTask> newDoingTasks;
    vector<Entity> newBuilders;
    for(int i = 0 ; i < playerView.entities.size(); i++) {
        const Entity& entity = playerView.entities[i];
        if (!Utils::isMyEntity(entity) || entity.entityType != BUILDER_UNIT) continue;
        newBuilders.push_back(entity);
        int id = entity.id;
        BuilderTask newTask;
        if (!doingTasks.count(id)) {
            //this is new builder, set current task to 'collect resource'
            newTask = BuilderTask(COLLECT_RESOURCE);
        } else {
            BuilderTask currentTask = doingTasks[id];
            newTask = currentTask; // keep old task if needed
            if (currentTask.taskType == COLLECT_RESOURCE) {
                // continue collecting resource
                newTask = currentTask;
            } else if (currentTask.taskType == BUILD) {
                // check if this building task is done or not
                int sz = Utils::getEntitySize(currentTask.targetEntityType);
                if (bitmap.isBuilt(currentTask.targetPosition, sz)) { // built
                    newTask = BuilderTask(COLLECT_RESOURCE);
                } else {
                    // task not change
                }
            } else if (currentTask.taskType == REPAIR) {
                // check if repairing job is done or not
                if (isInactiveEntity(currentTask.targetId)) {
                    //continue to repair
                } else {
                    newTask = BuilderTask(COLLECT_RESOURCE);
                }
            }
        }
        newDoingTasks[entity.id] = newTask;
    }
    builders = newBuilders;
}

void InformationForBuilderManager::update(const PlayerView &playerView) {
    int myId = playerView.myId;
    for (int i = 0; i < playerView.players.size(); i++) {
        if (playerView.players[i].id == myId) {
            currentResource = playerView.players[i].resource;
        }
    }
    curTick = playerView.currentTick;
    currentPopulation = 0;
    providedPopulation = 0;
    numberOfRangedBase = 0;
    numberOfMeleeBase = 0;
    numberOfBuilderBase = 0;
    for (int i = 0; i < playerView.entities.size(); i++) {
        const Entity &entity = playerView.entities[i];
        EntityType entityType = entity.entityType;
        if (!Utils::isMyEntity(entity)) continue;
        if (entity.entityType == BUILDER_UNIT) {
            builderPositions[entity.id] = entity.position;
        }
        currentPopulation += Utils::getEntityProperties(entityType).populationUse;
        providedPopulation += Utils::getEntityProperties(entityType).populationProvide;
        if (entityType == BUILDER_BASE) numberOfBuilderBase++;
        if (entityType == RANGED_BASE) numberOfRangedBase++;
        if (entityType == MELEE_BASE) numberOfMeleeBase++;
    }

    for (int i = 0; i < playerView.entities.size(); i++) {
        const Entity &entity = playerView.entities[i];
        if (Utils::isMyEntity(entity) && Utils::isInactiveEntity(entity)) {
            inactiveEntities.push_back(entity);
        }
        if (entity.entityType == RESOURCE) {
            resourcesEntities.push_back(entity);
        }
    }
    bitmap.update(playerView);
    updateBuilders(playerView);
}

int InformationForBuilderManager::getNumberOfBuilderRepairingFor(int entityId) {
    int res = 0;
    for(auto &p:doingTasks) {
        BuilderTask task = p.second;
        if (task.taskType == REPAIR && task.targetId == entityId) res++;
    }
    return res;
}
