//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_INFORMATIONFORATTACKERMANAGER_HPP
#define AICUP_CPP_INFORMATIONFORATTACKERMANAGER_HPP

#include "../../model/Model.hpp"


class TargetPriority {
    ATTACKER = 10,
    TURRET = 6,
    BUILDER = 4,
    HOUSE_AND_BASE = 1
};

class AttackerTaskType {

};

class AttackerTask {
public:
    AttackerTaskType attackerTaskType;

};

class InformationForAttackerManager {
public:
    InformationForAttackerManager();
    InformationForAttackerManager(const PlayerView& playerView);
    int curTick;
    vector<Entity> attackers;
    map<int, Vec2Int> attackersPositions;
    map<int, AttackerTask> doingTasks;
    vector<Entity> enemyAttackers;
    vector<Entity> enemyTurrets;
    vector<Entity> enemyBuilders;
    vector<Entity> enemyHouseAndBases;
    void update(const PlayerView& playerView);
};
#endif //AICUP_CPP_INFORMATIONFORATTACKERMANAGER_HPP
