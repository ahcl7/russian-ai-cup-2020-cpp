//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_INFORMATIONFORBUILDERMANAGER_HPP
#define AICUP_CPP_INFORMATIONFORBUILDERMANAGER_HPP

#include "model/Model.hpp"
#include "BitMap.hpp"
#include <vector>
#include <map>

using namespace std;

enum BuilderStatus {
    WAITING = 1,
    MOVING = 2,
    COLLECTING = 3,
    BUILDING = 4,
    REPAIRING = 5
};

enum TaskPriority {
    IMMEDIATE = 3,
    AS_SOON_AS_POSSIBLE = 2,
    CAN_BE_DELAYED = 1
};

enum TaskType {
    BUILD = 1,
    REPAIR = 2,
    COLLECT_RESOURCE = 3,
    NONE = 4;
};

class BuilderTask {
    TaskType taskType;
    int numberOfBuildersShouldBeInvolved;
    int targetId;
    EntityType targetEntityType;
    TaskPriority priority;
    Vec2Int targetPosition;
    BuilderTask(TaskType taskType1, int numberOfBuildersShouldBeInvolved1 = 1,  TaskPriority priority1 = CAN_BE_DELAYED, int targetId1 = -1, EntityType targetEntityType1 = HOUSE,
                Vec2Int targetPosition1 = SPECIAL_POINT) {
        taskType = taskType1;
        numberOfBuildersShouldBeInvolved = numberOfBuildersShouldBeInvolved1;
        targetId = targetId1;
        targetEntityType = targetEntityType1;
        priority = priority1;
        targetPosition = targetPosition1;
    }
};

class InformationForBuilderManager {
public:
    int currentResource;
    int currentPopulation;
    int providedPopulation;
    int curTick;
    int numberOfBuilderBase;
    int numberOfRangedBase;
    int numberOfMeleeBase;
    BitMap bitmap;
    map<int, Vec2Int> builderPositions;
    vector <Vec2Int> basePositions;
    map<int, BuilderStatus> status;
    map<int, BuilderTask> doingTasks;
    vector<Entity> builders;
    vector<Entity> inactiveEntities;
    vector<Entity> resourcesEntities;
    vector<BuilderTask> currentBuildingTasks;
    int getNumberOfBuilderRepairingFor(int entityId);
    InformationForBuilderManager(const PlayerView &playerView);

    void update(const PlayerView &playerView);
    void updateBuilders(const PlayerView& playerView);
    bool isInactiveEntity(int entityId);
};

#endif //AICUP_CPP_INFORMATIONFORBUILDERMANAGER_HPP
