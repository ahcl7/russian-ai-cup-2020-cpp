//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_ENTITYMANAGER_HPP
#define AICUP_CPP_ENTITYMANAGER_HPP

#include "BuilderStatus.hpp"
#include "AttackerStatis.hpp"
#include "
class EntityManager {
private:
    Vec2Int<Entity> entities;

public:
    void update(Vec2Int<Entity>& newEntities);

};

#endif //AICUP_CPP_ENTITYMANAGER_HPP
