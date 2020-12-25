//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_INFORMATIONFORATTACKERMANAGER_HPP
#define AICUP_CPP_INFORMATIONFORATTACKERMANAGER_HPP

#include "../../model/Model.hpp"
#include <vector>
#include <map>
#include "../../Utils.hpp"
#include <set>
#include <stdlib.h>

using namespace std;

enum TargetPriority {
    TG_ATTACKER = 10,
    TG_TURRET = 6,
    TG_BUILDER = 4,
    TG_HOUSE_AND_BASE = 1
};

enum LANES {
    TOP = 1,
    BOT = 2,
    DIA = 3
};
enum AttackerTaskType {
    ATTACK = 1,
    RUN = 2
};

class AttackerTask {
public:
    AttackerTaskType attackerTaskType;
    int targetId;
    Vec2Int targetPosition;
    AttackerTask() {};
    AttackerTask(AttackerTaskType taskType1, int targetId1 = -1, Vec2Int targetPosition1 = SPECIAL_POINT) {
        attackerTaskType = taskType1;
        targetId = targetId1;
        targetPosition = targetPosition1;
    }
};

class InformationForAttackerManager {
public:
    InformationForAttackerManager() {};
    InformationForAttackerManager(const PlayerView& playerView);
    bool started;
    int curTick;
    int numberOfPlayers;
    bool needAttackers;
    int numberOfBuilders;
    int numberOfRangedBase;
    vector<Entity> attackers;
    set<int> attackerIds;
    map<int, Vec2Int> attackersPositions;
    map<int, AttackerTask> doingTasks;
    vector<Entity> firstAttackerGroup;
    vector<Entity> secondAttackerGroup;
    vector<Entity> scouterGroup;
    vector<Entity> enemyAttackers; //including turrets
    vector<Entity> enemyBuilders;
    set<LANES> remainPlayers;
    vector<Entity> enemyHouseAndBases;
    bool isDead(LANES lane);
    void update(const PlayerView& playerView);
};
#endif //AICUP_CPP_INFORMATIONFORATTACKERMANAGER_HPP
