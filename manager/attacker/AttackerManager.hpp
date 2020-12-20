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

using namespace std;
class AttackerManager {
public:

    AttackerManager(){};
    AttackerManager(const PlayerView& playerView);
    InformationForAttackerManager infoFAM;
    EntityAction getAction(int entityId);
    void update(const PlayerView& playerView);
    void createTask();
};
#endif //AICUP_CPP_ATTACKERMANAGER_HPP
