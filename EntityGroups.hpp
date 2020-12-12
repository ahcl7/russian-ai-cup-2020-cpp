//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_ENTITYGROUPS_HPP
#define AICUP_CPP_ENTITYGROUPS_HPP

#include <vector>
#include "model/Model.hpp"

class EntityGroups {
public:
    std::vector<Entity> builders;
    std::vector<Entity> rangedUnits;
    std::vector<Entity> meleeUnits;
    void update(const PlayerView& playerView);
};
#endif //AICUP_CPP_ENTITYGROUPS_HPP
