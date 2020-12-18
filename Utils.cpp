//
// Created by Apple on 14/11/2020.
//

#include "Utils.hpp"
#include "model/Model.hpp"

int Utils::distance(Vec2Int a, Vec2Int b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}
int Utils::distance(Entity a, Entity b) {
    return distance(a.position, b.position);
}
int Utils::sqr(int x) {
    return x * x;
}
int Utils::distance2(Vec2Int a, Vec2Int b) {
    return sqr(a.x - b.x) + sqr(a.y - b.y);
}
bool Utils::intersect(int l, int r, int u, int v) {
    int L = std::max(l, u);
    int R = std::min(r, v);
    return (L <= R);
}
bool Utils::intersect(Vec2Int a, int sa, Vec2Int b, int sb) {
    bool res = intersect(a.x, a.x + sa - 1, b.x, b.x + sb - 1);
    res &= intersect(a.y, a.y + sa - 1, b.y, b.y + sb - 1);
    return res;
}

bool Utils::isAdjacent(Vec2Int a, Vec2Int b, int sz) {
    for (int i = 0; i < sz; i++) {
        for (int j = 0; j < sz; j++) {
            int x = b.x + i;
            int y = b.y + j;
            if (abs(a.x - x) + abs(a.y - y) == 1) return true;
        }
    }
    return false;
}

Vec2Int Utils::getTopCoordinate() {
    return Vec2Int(BASE_CENTER, mapSize - BASE_CENTER);
}

Vec2Int Utils::getBotCoordinate() {
    return Vec2Int(mapSize - BASE_CENTER, BASE_CENTER);
}

Vec2Int Utils::getDiaCoordinate() {
    return Vec2Int(mapSize - BASE_CENTER, mapSize - BASE_CENTER);
}

Vec2Int Utils::getBaseCoordinate() {
    return Vec2Int(BASE_CENTER, BASE_CENTER);
}

Vec2Int Utils::getCenterCoordinate() {
    return Vec2Int(mapSize / 2, mapSize / 2);
}

bool Utils::isMyEntity(Entity entity) {
    return (entity.playerId != nullptr && entity.id == myId);
}

bool Utils::isInactiveEntity(Entity entity) {
    EntityType entityType = entity.entityType;
    return entity.health < entityProperties.at(entityType).maxHealth;
}

Vec2Int Utils::getResourceOptimalCoordinate() {
    return Vec2Int(mapSize / 2, mapSize / 2);
}
int Utils::getEntitySize(EntityType entityType) {
    return entityProperties.at(entityType).size;
}