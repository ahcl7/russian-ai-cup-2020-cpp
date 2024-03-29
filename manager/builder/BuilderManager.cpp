//
// Created by Apple on 14/11/2020.
//

#include "BuilderManager.hpp"
#include <iostream>

BuilderManager::BuilderManager(const PlayerView &playerView) {
    infoFBM = InformationForBuilderManager(playerView);
    this->mcmf = MCMF();
    this->update(playerView);

}

Vec2Int BuilderManager::getClosetResource(Vec2Int position, vector<Entity> resourceEntities) {
    int MinDis = INF;
    Vec2Int res = Utils::getResourceOptimalCoordinate();
    for (auto &p:resourceEntities) {
        bool ok = false;
        for (int i = -1; i < 2 && !ok; i++) {
            for (int j = -1; j < 2 && !ok; j++) {
                if (abs(i) + abs(j) == 1) {
                    int x = p.position.x + i;
                    int y = p.position.y + j;
                    if (Vec2Int(x, y) == position) {
                        ok = true;
                        break;
                    }
                    if (x < 0 || y < 0 || x >= Utils::mapSize - 1 || y >= Utils::mapSize - 1) continue;
                    ok |= (this->infoFBM.bitmap.isFowEnable ? this->infoFBM.bitmap._bitmap_fow[x][y] == EMPTY
                                                            : this->infoFBM.bitmap._bitmap[x].test(y) == 0);
                }
            }
        }
        if (ok && Utils::distance(position, p.position) < MinDis) {
            MinDis = Utils::distance(position, p.position);
            res = p.position;
        }
    }
    return res;
}

EntityAction BuilderManager::getAction(int entityId) {
    // get current task entityId is doing
    //

    // every entity should be assigned a task before getAction is invoked
    BuilderTask currentTask = this->infoFBM.doingTasks[entityId];

    shared_ptr<MoveAction> moveAction = nullptr;
    shared_ptr<BuildAction> buildAction = nullptr;
    shared_ptr<AttackAction> attackAction = nullptr;
    shared_ptr<RepairAction> repairAction = nullptr;
    if (currentTask.taskType == COLLECT_RESOURCE) {
        if (this->infoFBM.resourcesEntities.size() > 0) {
            //TODO: upgrade strategy: closest resource may be blocked by other builders, this cause waste of resource.
            //TODO: can limit number of builders, many builder is not really good because cost is increasing rapidly
            //TODO: keep the number of builder = number of attackers = ranged + melee (ranged should be more than melee)
            Vec2Int p = getClosetResource(this->infoFBM.builderPositions[entityId], this->infoFBM.resourcesEntities);
            if (Utils::distance(this->infoFBM.builderPositions[entityId], p) > RESOURCE_RANGE) {
                moveAction = shared_ptr<MoveAction>(new MoveAction(p, true, true));

            } else {
                // TODO: make a move if builder is not moving
            }
        } else {
            //move around
            moveAction = shared_ptr<MoveAction>(new MoveAction(Utils::getResourceOptimalCoordinate(), true, true));
        }
        vector<EntityType> targets;
        targets.push_back(RESOURCE);
        shared_ptr<AutoAttack> autoAttack = shared_ptr<AutoAttack>(new AutoAttack(10, targets));
        attackAction = shared_ptr<AttackAction>(new AttackAction(nullptr, autoAttack));
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
        if (this->infoFBM.builderPositions[entityId] ==
            Vec2Int(currentTask.targetPosition.x - 1, currentTask.targetPosition.y)) {
            buildAction = shared_ptr<BuildAction>(
                    new BuildAction(currentTask.targetEntityType, currentTask.targetPosition));
        } else {
            int sz = Utils::getEntitySize(currentTask.targetEntityType);
            Vec2Int centerPosition = Vec2Int(currentTask.targetPosition.x - 1,
                                             currentTask.targetPosition.y);
            moveAction = shared_ptr<MoveAction>(new MoveAction(centerPosition, true, true));
        }
    }

    return EntityAction(moveAction, buildAction, attackAction, repairAction);
};

void BuilderManager::implement(vector<Entity> &buildersCanBeInvolvedForTasks,
                               vector<BuilderTask> &tasks, vector<pair<int, int>> &assignment) {
    for (int i = 0; i < assignment.size(); i++) {
        int u = assignment[i].first;
        int v = assignment[i].second;
        int builderId = buildersCanBeInvolvedForTasks[u].id;
        BuilderTask task = tasks[v];
        infoFBM.doingTasks[builderId] = task;
    }
}

void BuilderManager::assignTasks(vector<BuilderTask> &tasks) {
    vector<Entity> buildersCanBeInvolvedForTasks;

    int totalBuilderCnt = this->infoFBM.builders.size();
    for (int i = 0; i < this->infoFBM.builders.size(); i++) {
        BuilderTask currentTask = this->infoFBM.doingTasks[this->infoFBM.builders[i].id];
        if (currentTask.taskType == COLLECT_RESOURCE) {
            buildersCanBeInvolvedForTasks.push_back(this->infoFBM.builders[i]);
        }
    }
    sort(tasks.begin(), tasks.end(), [](BuilderTask a, BuilderTask b) {
        return a.priority > b.priority;
    });
    vector<EntityType> basesBuildingTasks;
    for (int i = 0; i < tasks.size(); i++) {
        if (tasks[i].taskType == BUILD) {
            basesBuildingTasks.push_back(tasks[i].targetEntityType);
        }
    }
    vector<Vec2Int> positions = infoFBM.bitmap.getBestPositions(basesBuildingTasks);
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
    int ss1 = ss + 1;
    int tt = ss1 + 1;
    cerr << "cnt1 " << tt << endl;
    this->mcmf.init(tt + 1, ss1, tt);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (!(tasks[j].targetPosition == SPECIAL_POINT)) {
                this->mcmf.add(i, j + n, 1,
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
        this->mcmf.add(j + n, tt, tasks[j].numberOfBuildersShouldBeInvolved, cost);
    }

    for (int i = 0; i < n; i++) {
        this->mcmf.add(ss, i, 1, 0);
    }
    int numberOfBuilderIsDoingHardTask = totalBuilderCnt - buildersCanBeInvolvedForTasks.size();
    this->mcmf.add(ss1, ss, max(0, min(20, totalBuilderCnt / 2 - numberOfBuilderIsDoingHardTask)), 0);

    vector<pair<int, int>> assignment = this->mcmf.getPairs(n, m);
    implement(buildersCanBeInvolvedForTasks, tasks, assignment);
}

void BuilderManager::createAndAssignTasks() {

    // house, bases, resources
    int remainProvidedPopulation = infoFBM.providedPopulation - infoFBM.currentPopulation;
    int numberOfHouseCanBeBuilt = infoFBM.currentResource / Utils::getEntityCost(HOUSE);
    int expectedNumberOfHouse = numberOfHouseCanBeBuilt;
    expectedNumberOfHouse = min(expectedNumberOfHouse, min(3, max(0, 5 - remainProvidedPopulation / 5)));
    if (this->infoFBM.needRangedBase) expectedNumberOfHouse = 0;
    //TODO: should count number of houses is building
    int numberOfHouseIsBuilding = infoFBM.getNumberOfHouseIsBuilding();
    expectedNumberOfHouse = max(0, expectedNumberOfHouse - numberOfHouseIsBuilding);
    //involve builder to repair inactive entities
    vector<BuilderTask> tasks;
    for (int i = 0; i < infoFBM.inactiveEntities.size(); i++) {
        EntityType entityType = infoFBM.inactiveEntities[i].entityType;
        int numberOfBuilderShouldBeInvolved = entityType == HOUSE ? NUMBER_OF_BUILDER_FOR_HOUSE :
                                              (entityType == TURRET ? NUMBER_OF_BUILDER_FOR_TURRET
                                                                    : NUMBER_OF_BUILDER_FOR_BASES);
        TaskPriority priority = (entityType == HOUSE ? AS_SOON_AS_POSSIBLE : CAN_BE_DELAYED);
        int currentRepairingNumber = infoFBM.getNumberOfBuilderRepairingFor(infoFBM.inactiveEntities[i].id);
        int expectedNumberOfBuilderForRepair = max(0,
                                                   min(infoFBM.currentPopulation / 2, numberOfBuilderShouldBeInvolved) -
                                                   currentRepairingNumber);
        tasks.push_back(BuilderTask(REPAIR, expectedNumberOfBuilderForRepair, priority,
                                    infoFBM.inactiveEntities[i].id, infoFBM.inactiveEntities[i].entityType,
                                    infoFBM.inactiveEntities[i].position));
    }
    for (int i = 0; i < expectedNumberOfHouse; i++)
        tasks.push_back(BuilderTask(BUILD, 1, AS_SOON_AS_POSSIBLE, -1, HOUSE));
    // TODO: need some logic here
    bool needBuilderBase = false;



//    needRangedBase |= (this->infoFBM.currentResource > 4 * Utils::getEntityCost(RANGED_BASE)); // too much resource
//    needRangedBase &= (this->infoFBM.numberOfRangedBase + this->infoFBM.currentBuilderBuildingRangedBase <= 3);
    bool needMeleeBase = false;
    if (needBuilderBase) {
        tasks.push_back(BuilderTask(BUILD, 1, IMMEDIATE, -1, BUILDER_BASE));
    }
    if ((this->infoFBM.needRangedBase && this->infoFBM.currentResource >= Utils::getEntityCost(RANGED_BASE)
         || this->infoFBM.currentResource > 2500) &&
        this->infoFBM.currentBuilderBuildingRangedBase == 0) {
        tasks.push_back(BuilderTask(BUILD, 1, CAN_BE_DELAYED, -1, RANGED_BASE));
    }

    if ((this->infoFBM.needTurret)) {
        tasks.push_back(BuilderTask(BUILD, 1, CAN_BE_DELAYED, -1, TURRET));
    }
    if (needMeleeBase) {
        tasks.push_back(BuilderTask(BUILD, 1, CAN_BE_DELAYED, -1, MELEE_BASE));
    }
    assignTasks(tasks);
};

void BuilderManager::update(const PlayerView &playerView) {
    infoFBM.update(playerView);
    createAndAssignTasks();
};