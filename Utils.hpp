//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_UTILS_HPP
#define AICUP_CPP_UTILS_HPP

#include <unordered_map>
#include "model/Model.hpp"

using namespace std;

namespace Utils {
    int mapSize = 0;
    int myId = -1;
    unordered_map<EntityType, EntityProperties> entityProperties;
    int distance(Vec2Int a, Vec2Int b);
    int distance(Entity a, Entity b);
    int sqr(int x);
    bool intersect(int l, int r, int u, int v);
    bool intersect(Vec2Int a, int sa, Vec2Int b, int sb);
    bool isAdjacent(Vec2Int a, Vec2Int b, int sz);
    Vec2Int getTopCoordinate();
    Vec2Int getBotCoordinate();
    Vec2Int getDiaCoordinate();
    Vec2Int getBaseCoordinate();
    Vec2Int getCenterCoordinate();
    Vec2Int getResourceOptimalCoordinate();
    int getEntitySize(EntityType entityType);
    bool isMyEntity(Entity entity);
    bool isInactiveEntity(Entity entity);
};

#endif //AICUP_CPP_UTILS_HPP
