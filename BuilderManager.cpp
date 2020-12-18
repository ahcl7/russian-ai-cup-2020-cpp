//
// Created by Apple on 14/11/2020.
//

#include "BuilderManager.hpp"

BuilderManager::BuilderManager(const PlayerView& playerView) {
    infoFBM = InformationForBuilderManager(playerView);
    this->update(playerView);
}

Entity getClosetResource(Vec2Int position, vector <Entity> resourceEntities) {
    int MinDis = INF;
    Entity res;
    for (auto &p:resourceEntities) {
        if (Utils::distance(position, p.position) < MinDis) {
            MinDis = Utils::distance(position, p.position);
            res = p;
        }
    }
    return res;
}

EntityAction BuilderManager::getAction(int entityId) {
    // get current task entityId is doing
    //

    // every entity should be assigned a task before getAction is invoked
    BuilderTask currentTask = this->infoFBM.doingTasks[entityId];
    shared_ptr <MoveAction> moveAction = nullptr;
    shared_ptr <BuildAction> buildAction = nullptr;
    shared_ptr <AttackAction> attackAction = nullptr;
    shared_ptr <RepairAction> repairAction = nullptr;

    if (currentTask.taskType == COLLECT_RESOURCE) {
        // continue collect resource

        // find the closest resource
        if (this->infoFBM.resourcesEntities.size() > 0) {
            Entity p = getClosetResource(this->infoFBM.builderPositions[entityId], this->infoFBM.resourcesEntities);
            if (Utils::distance(this->infoFBM.builderPositions[entityId], p.position) > RESOURCE_RANGE) {
                moveAction = shared_ptr<MoveAction>(new MoveAction(p.position, true, true));
            } else {
                // TODO: make a move if builder is not moving
            }
        } else {
            //move around
            moveAction = shared_ptr<MoveAction>(new MoveAction(Utils::getResourceOptimalCoordinate(), true, true));
        }
    }
    if (currentTask.taskType == REPAIR) {
        EntityType entityType = currentTask.targetEntityType;
        int sz = Utils::getEntitySize(entityType);
        //builder is adjacent to base
        if (Utils::isAdjacent(this->infoFBM.builderPositions[entityId], currentTask.targetPosition, sz)) {
            repairAction = shared_ptr<RepairAction>(new RepairAction(currentTask.targetId));
        } else {
            Vec2Int centerPosition = Vec2Int(currentTask.targetPosition.x + sz / 2,
                                             currentTask.targetPosition.y + sz / 2);
            moveAction = shared_ptr<MoveAction>(new MoveAction(centerPosition, true, true));
        }
    }

    if (currentTask.taskType == BUILD) {
        buildAction = shared_ptr<BuildAction>(
                new BuildAction(currentTask.targetEntityType, currentTask.targetPosition));
    }

    return EntityAction(moveAction, buildAction, attackAction, repairAction);
};

void BuilderManager::implement(vector <Entity> buildersCanBeInvolvedForTasks,
                               vector <BuilderTask> tasks, vector <pair<int, int>> assignment) {
    for (int i = 0; i < assignment.size(); i++) {
        int u = assignment[i].first;
        int v = assignment[i].second;
        int builderId = buildersCanBeInvolvedForTasks[u].id;
        BuilderTask task = tasks[v];
        infoFBM.doingTasks[builderId] = task;
    }
}

void BuilderManager::assignTasks(vector <BuilderTask> tasks) {
    vector <Entity> buildersCanBeInvolvedForTasks;
    for (int i = 0; i < this->infoFBM.builders.size(); i++) {
        BuilderTask currentTask = this->infoFBM.doingTasks[this->infoFBM.builders[i].id];
        if (currentTask.taskType != REPAIR) {
            buildersCanBeInvolvedForTasks.push_back(this->infoFBM.builders[i]);
        }
    }
    sort(tasks.begin(), tasks.end(), [](BuilderTask a, BuilderTask b) {
        return a.priority > b.priority;
    });
    // need cost function here
    vector <EntityType> basesBuildingTasks;
    for (int i = 0; i < tasks.size(); i++) {
        if (tasks[i].taskType == BUILD) {
            basesBuildingTasks.push_back(tasks[i].targetEntityType);
        }
    }
    vector <Vec2Int> positions = infoFBM.bitmap.getBestPositions(basesBuildingTasks);
    int cur = 0;
    for (int i = 0; i < tasks.size(); i++) {
        if (tasks[i].taskType == BUILD) {
            tasks[i].targetPosition = positions[cur++];
        }
    }

    //build min cost max flow graph
    int n = buildersCanBeInvolvedForTasks.size();
    int m = tasks.size();
    int ss = n + m;
    int tt = ss + 1;
    MCMF::init(tt + 1, ss, tt);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (!(tasks[j].targetPosition == SPECIAL_POINT)) {
                MCMF::add(i, j + n, 1,
                          Utils::distance(buildersCanBeInvolvedForTasks[i].position, tasks[j].targetPosition));
            }
        }
    }
    for (int j = 0; j < m; j++) {
        int cost;
        TaskPriority priority = tasks[j].priority;
        if (priority == IMMEDIATE) {
            cost = 0;
        } else if (priority == AS_SOON_AS_POSSIBLE) {
            cost = 5;
        } else cost = 7;
        MCMF::add(j + n, tt, tasks[j].numberOfBuildersShouldBeInvolved, cost);
    }
    for (int i = 0; i < n; i++) {
        MCMF::add(ss, i, 1, 0);
    }

    vector <pair<int, int>> assignment = MCMF::getPairs(n, m);
    implement(buildersCanBeInvolvedForTasks, tasks, assignment);
}

void BuilderManager::createAndAssignTasks() {
    // house, bases, resources
    int remainProvidedPopulation = infoFBM.providedPopulation - infoFBM.currentPopulation;
    int numberOfHouseCanBeBuilt = infoFBM.currentResource / Utils::getEntityCost(HOUSE);
    int expectedNumberOfHouse = numberOfHouseCanBeBuilt;
    expectedNumberOfHouse = min(expectedNumberOfHouse, min(3, max(0, 5 - remainProvidedPopulation / 5)));

    //involve builder to repair inactive entities
    vector <BuilderTask> tasks;
    for (int i = 0; i < infoFBM.inactiveEntities.size(); i++) {
        if (infoFBM.inactiveEntities[i].entityType == HOUSE) {


            int expectedNumberOfBuilderForRepair = min(infoFBM.currentPopulation / 2, NUMBER_OF_BUILDER_FOR_HOUSE);
            tasks.push_back(BuilderTask(REPAIR, expectedNumberOfBuilderForRepair, AS_SOON_AS_POSSIBLE,
                                        infoFBM.inactiveEntities[i].id, infoFBM.inactiveEntities[i].entityType,
                                        infoFBM.inactiveEntities[i].position));
        } else {
            //TODO: should count number of builder is repairing this base
            int currentRepairingNumber = infoFBM.getNumberOfBuilderRepairingFor(infoFBM.inactiveEntities[i].id);
            int expectedNumberOfBuilderForRepair = max(0, min(infoFBM.currentPopulation / 2, NUMBER_OF_BUILDER_FOR_BASES) - currentRepairingNumber);
            tasks.push_back(BuilderTask(REPAIR, expectedNumberOfBuilderForRepair, CAN_BE_DELAYED,
                                        infoFBM.inactiveEntities[i].id, infoFBM.inactiveEntities[i].entityType,
                                        infoFBM.inactiveEntities[i].position));
        }
    }
    for (int i = 0; i < expectedNumberOfHouse; i++) tasks.push_back(BuilderTask(BUILD, 1));
    // TODO: need some logic here
    bool needBuilderBase = false;
    bool needRangedBase = false;
    bool needMeleeBase = false;
    if (needBuilderBase) {
        tasks.push_back(BuilderTask(BUILD, 1, IMMEDIATE, -1, BUILDER_BASE));
    }
    if (needRangedBase) {
        tasks.push_back(BuilderTask(BUILD, 1, CAN_BE_DELAYED, -1, RANGED_BASE));
    }
    if (needMeleeBase) {
        tasks.push_back(BuilderTask(BUILD, 1, CAN_BE_DELAYED, -1, MELEE_BASE));
    }
    assignTasks(tasks);
};

void BuilderManager::update(const PlayerView &playerView) {
    infoFBM.update(playerView);
};