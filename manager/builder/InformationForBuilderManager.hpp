//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_INFORMATIONFORBUILDERMANAGER_HPP
#define AICUP_CPP_INFORMATIONFORBUILDERMANAGER_HPP

#include "../../model/Model.hpp"
#include "../bitmap/BitMap.hpp"
#include <vector>
#include <map>
#include <iostream>

using namespace std;

enum TaskPriority {
    IMMEDIATE = 3,
    AS_SOON_AS_POSSIBLE = 2,
    CAN_BE_DELAYED = 1
};

enum TaskType {
    BUILD = 1,
    REPAIR = 2,
    COLLECT_RESOURCE = 3,
    NONE = 4
};

class BuilderTask {
public:
    TaskType taskType;
    int numberOfBuildersShouldBeInvolved;
    int targetId;
    EntityType targetEntityType;
    TaskPriority priority;
    Vec2Int targetPosition;
    BuilderTask() {
        taskType = NONE;
        numberOfBuildersShouldBeInvolved = 0;
        targetId = -1;
        targetEntityType = HOUSE;
        priority = CAN_BE_DELAYED;
        targetPosition = SPECIAL_POINT;
    }
    BuilderTask(TaskType taskType1, int numberOfBuildersShouldBeInvolved1 = 1,  TaskPriority priority1 = CAN_BE_DELAYED, int targetId1 = -1, EntityType targetEntityType1 = HOUSE,
                Vec2Int targetPosition1 = SPECIAL_POINT) {
        taskType = taskType1;
        numberOfBuildersShouldBeInvolved = numberOfBuildersShouldBeInvolved1;
        targetId = targetId1;
        targetEntityType = targetEntityType1;
        priority = priority1;
        targetPosition = targetPosition1;
    }
    friend ostream& operator<<(ostream& out, const BuilderTask& builderTask) {
        out << (builderTask.taskType == BUILD ? "BUILD" : (builderTask.taskType == REPAIR ? "REPAIR" : "RESOURCE")) <<" " <<
        builderTask.numberOfBuildersShouldBeInvolved << " "<< (builderTask.targetEntityType == HOUSE ? "HOUSE" : "BASE");
        out << builderTask.targetPosition.x <<" " << builderTask.targetPosition.y;
        return out;
    }
};

class InformationForBuilderManager {
public:
    int currentResource;
    int numberOfAttackers;
    int currentPopulation;
    int providedPopulation;
    int curTick;
    int numberOfBuilderBase;
    int numberOfRangedBase;
    int numberOfMeleeBase;
    int currentBuilderBuildingRangedBase;
    bool needRangedBase;
    BitMap bitmap;
    map<int, Vec2Int> builderPositions;
    map<int, BuilderTask> doingTasks;
    vector<Entity> builders;
    vector<Entity> inactiveEntities;
    vector<Entity> resourcesEntities;
    int getNumberOfBuilderRepairingFor(int entityId);
    int getNumberOfHouseIsBuilding();
    InformationForBuilderManager() {};
    InformationForBuilderManager(const PlayerView &playerView);
    void update(const PlayerView &playerView);
    void updateBuilders(const PlayerView& playerView);
    bool isInactiveEntity(int entityId);
    friend ostream& operator<<(ostream& os, const InformationForBuilderManager& info) {
        os << "curTick " << info.curTick << endl;
        os << "current population: " << info.currentPopulation <<endl;
        os << "providedPopulation" << info.providedPopulation << endl;
        return os;
    }
};

#endif //AICUP_CPP_INFORMATIONFORBUILDERMANAGER_HPP
