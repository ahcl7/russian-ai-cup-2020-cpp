//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_ATTACKERMANAGER_HPP
#define AICUP_CPP_ATTACKERMANAGER_HPP

#include "model/Model.hpp"

class AttackerManager {
public:
    std::vector<Entity> attackers;
    std::vector<Entity> newAttackers;
    EntityAction getAction(int entityId);
    void update(std::vector<Entity> newAttackers);
    void createTask();
};
#endif //AICUP_CPP_ATTACKERMANAGER_HPP
