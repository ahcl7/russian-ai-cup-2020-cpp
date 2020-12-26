//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP2020_BASEMANAGER_HPP
#define AICUP2020_BASEMANAGER_HPP

#endif //AICUP2020_BASEMANAGER_HPP

#include "InformationForBaseManager.hpp"
class BaseManager {
public:
    InformationForBaseManager infoFBM;
    BaseManager() {};
    BaseManager(const PlayerView& playerView);
    void update(const PlayerView& playerView);
    EntityAction getAction(const Entity& entity, bool needRangedBase, bool  needAttackers);
};