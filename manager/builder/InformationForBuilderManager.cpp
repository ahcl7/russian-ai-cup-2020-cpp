//
// Created by Apple on 14/11/2020.
//
#include "InformationForBuilderManager.hpp"

InformationForBuilderManager::InformationForBuilderManager(const PlayerView &playerView) {
    currentResource = 0;
    currentPopulation = 0;
    providedPopulation = 0;
    numberOfBuilderBase = 0;
    numberOfMeleeBase = 0;
    numberOfRangedBase = 0;
    curTick = 0;
    numberOfAttackers = 0;
    bitmap = BitMap(playerView.mapSize);
};

bool InformationForBuilderManager::isInactiveEntity(int entityId) {
    for (int i = 0; i < inactiveEntities.size(); i++) {
        if (inactiveEntities[i].id == entityId) return true;
    }
    return false;
}

void InformationForBuilderManager::updateBuilders(const PlayerView &playerView) {
    unordered_map<int, BuilderTask> newDoingTasks;
    vector<Entity> newBuilders;
    for (int i = 0; i < playerView.entities.size(); i++) {
        const Entity &entity = playerView.entities[i];
        if (!Utils::isMyEntity(entity) || entity.entityType != BUILDER_UNIT) continue;
        newBuilders.push_back(entity);
        int id = entity.id;
        BuilderTask newTask;
        //TODO: find builders are standing and doing nothing => force him collecting resources
        //TODO: run if have an attacker can target this builder
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
        newDoingTasks[id] = newTask;
    }

    //update builders and doing tasks
    doingTasks = newDoingTasks;
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
    numberOfAttackers = 0;
    currentBuilderBuildingRangedBase = 0;
    builderPositions.clear();
    inactiveEntities.clear();
    resourcesEntities.clear();
    for (int i = 0; i < playerView.entities.size(); i++) {
        const Entity &entity = playerView.entities[i];
        EntityType entityType = entity.entityType;
        if (!Utils::isMyEntity(entity)) continue;
        if (entity.entityType == BUILDER_UNIT) {
            builderPositions[entity.id] = entity.position;
        }
        if (entityType == RANGED_UNIT || entityType == MELEE_UNIT) numberOfAttackers++;
        currentPopulation += Utils::getEntityProperties(entityType).populationUse;
        providedPopulation += Utils::getEntityProperties(entityType).populationProvide;
        if (entityType == BUILDER_BASE) numberOfBuilderBase++;
        if (entityType == RANGED_BASE) numberOfRangedBase++;
        if (entityType == MELEE_BASE) numberOfMeleeBase++;
    }

    for (int i = 0; i < playerView.entities.size(); i++) {
        const Entity &entity = playerView.entities[i];
        // inactive entities include all entity that current health < max health, exclude all units
        if (Utils::isMyEntity(entity) && Utils::isInactiveEntity(entity) &&
            !Utils::getEntityProperties(entity.entityType).canMove) {
            inactiveEntities.push_back(entity);
            if (entity.entityType == RANGED_BASE) {
                currentBuilderBuildingRangedBase++;
            }
        }
        if (entity.entityType == RESOURCE) {
            resourcesEntities.push_back(entity);
        }
    }
    bitmap.update(playerView);
    // cover all in building bases/houses
    for (auto &p: doingTasks) {
        BuilderTask task = p.second;
        if (task.taskType == BUILD) {
            bitmap.cover(task.targetPosition, Utils::getEntitySize(task.targetEntityType));
            if (task.targetEntityType == RANGED_BASE) {
                currentBuilderBuildingRangedBase++;
            }
        }
    }
//    needRangedBase = (numberOfRangedBase + currentBuilderBuildingRangedBase == 0) && builderPositions.size() >= MIN_BUILDERS;
    needRangedBase = (numberOfRangedBase == 0) && builderPositions.size() >= MIN_BUILDERS;
    needTurret = (this->curTick > 100 && (((this->curTick - 100) % 40 == 0) || (this->curTick - 100) % 25) == 0 &&
                  this->currentResource > 500);
    cerr << *this << endl;
    updateBuilders(playerView);
}

int InformationForBuilderManager::getNumberOfBuilderRepairingFor(int entityId) {
    int res = 0;
    for (auto &p:doingTasks) {
        BuilderTask task = p.second;
        if (task.taskType == REPAIR && task.targetId == entityId) res++;
    }
    return res;
}

int InformationForBuilderManager::getNumberOfHouseIsBuilding() {
    int res = 0;
    for (auto &p:doingTasks) {
        if (p.second.taskType == BUILD) res++;
    }
    return res;
}
