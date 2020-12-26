//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_ATTACKERMANAGER_HPP
#define AICUP_CPP_ATTACKERMANAGER_HPP

#include "../../model/Model.hpp"
#include <map>
#include <iostream>
#include <vector>
#include "InformationForAttackerManager.hpp"
#include "../../mcmf.hpp"
#include "../../Constances.hpp"
#include <assert.h>

using namespace std;
class AttackerManager {
public:
    MCMF mcmf;
    MCMF mcmf1;
    AttackerManager(){};
    AttackerManager(const PlayerView& playerView);
    InformationForAttackerManager infoFAM;
    EntityAction getAction(int entityId);
    void update(const PlayerView& playerView);
    void createTask();
    vector<Entity> attack(vector<Entity>& attackers, Vec2Int targetPosition);
    bool getBack(vector<Entity> attackers);
};
#endif //AICUP_CPP_ATTACKERMANAGER_HPP
