//
// Created by Apple on 14/11/2020.
//

#include "BitMap.hpp"

BitMap::BitMap(int n1) {
    n = n1;
    for (int i = 0; i < n; i++) {
        bitmap.push_back(bitset<MAX_SIZE>(0));
        bitmap_fow.push_back(vector<CellStatus>(MAX_SIZE, UNKNOWN));
    }
    _bitmap = bitmap;
    _bitmap_fow = bitmap_fow;
}

void BitMap::update(const PlayerView &playerView) {
    isFowEnable = playerView.fogOfWar;
    for (int i = 0; i < n; i++) {
        bitmap[i].reset();
        bitmap_fow[i] = vector<CellStatus>(MAX_SIZE, UNKNOWN);
    }
    for (int i = 0; i < playerView.entities.size(); i++) {
        const Entity &entity = playerView.entities[i];
        EntityType entityType = entity.entityType;
        Vec2Int position = entity.position;
        int entitySize = Utils::getEntitySize(entityType);

        // mark all cell which is covered by a base/house to true
        for (int j = 0; j < entitySize; j++) {
            for (int k = 0; k < entitySize; k++) {
                bitmap[position.x + j].set(position.y + k);
            }
        }

        // mark all cell which is covered by a base/house to 'OCCUPIED'
        if (!Utils::getEntityProperties(entityType).canMove) {
            for (int j = 0; j < entitySize; j++) {
                for (int k = 0; k < entitySize; k++) {
                    bitmap_fow[position.x + j][position.y + k] = OCCUPIED;
                }
            }
        }
    }

    // mark all unknown cell inside current sight range to emtpy
    for (int i = 0; i < playerView.entities.size(); i++) {
        const Entity &entity = playerView.entities[i];
        EntityType entityType = entity.entityType;
        Vec2Int position = entity.position;
        int entitySize = Utils::getEntitySize(entityType);
        if (!Utils::isMyEntity(entity)) {
            int sightRange = Utils::getEntitySightRange(entityType);
            for (int j = -sightRange; j < sightRange + entitySize; j++) {
                for (int k = -sightRange; k < sightRange + entitySize; k++) {
                    int x = position.x + j;
                    int y = position.y + k;
                    if (x >= 0 && y >= 0 && x < n && y < n) {
                        if (bitmap_fow[x][y] == UNKNOWN) {
                            bitmap_fow[x][y] = EMPTY;
                        }
                    }
                }
            }
        }
    }
    _bitmap = bitmap;
    _bitmap_fow = bitmap_fow;
};

bool BitMap::canBuild(Vec2Int position, int entitySize) {
    for (int k = 0; k < entitySize; k++) {
        for (int l = 0; l < entitySize; l++) {
            if (position.x + k >= n || position.y + l >= n) return false;
            if (!isFowEnable) {
                if (_bitmap[position.x + k].test(position.y + l)) return false;
            } else if (_bitmap_fow[position.x + k][position.y + l] != EMPTY) return false;
        }
    }
    return true;
}

bool BitMap::isBuilt(Vec2Int position, int entitySize) {
    int cnt = 0;
    for (int i = 0; i < entitySize; i++) {
        for (int j = 0; j < entitySize; j++) {
            cnt += (!isFowEnable) ? bitmap[position.x + i].test(position.y + j)
                                  : (bitmap_fow[position.x + i][position.y + j] == OCCUPIED);
        }
    }
    return cnt == Utils::sqr(entitySize);
}

vector <Vec2Int> BitMap::getPositionsForNewEntity(EntityType entityType) {
    int entitySize = entityProperties.at(entityType).size;
    entitySize++; // expand entitySize 1
    vector <Vec2Int> res;
    for (int i = 1; i < Utils::mapSize - entitySize; i++) {
        for (int j = 1; j < Utils::mapSize - entitySize; j++) {
            if (canBuild(Vec2Int(i, j), entitySize)) res.push_back(Vec2Int(i, j));
        }
    }
    return res;
}

vector <Vec2Int> BitMap::getBestPositions(vector <EntityType> entityTypes) {
    int houseSize = Utils::getEntitySize(HOUSE) + 1;
    int otherBaseSize = Utils::getEntitySize(RANGED_BASE) + 1;
    auto cmp = [](Vec2Int a, Vec2Int b) {
        int t = a.x + a.y;
        int t1 = b.x + b.y;
        if (t != t1) return t < t1;
        return make_pair(a.x, a.y) < make_pair(b.x, b.y);
    };
    set<Vec2Int, decltype(cmp)> se(cmp);
    for (auto p:getPositionsForNewEntity(HOUSE)) se.insert(p);
    set<Vec2Int, decltype(cmp)> se1(cmp);
    for (auto p:getPositionsForNewEntity(RANGED_BASE)) se1.insert(p);

    vector <Vec2Int> res;
    for (int i = 0; i < entityTypes.size(); i++) {
        EntityType entityType = entityTypes[i];
        if (entityType == HOUSE) {
            if (se.empty()) {
                res.push_back(SPECIAL_POINT);
            } else {
                Vec2Int tmp = *se.begin();
                res.push_back(tmp);
                for (int j = 0; j < houseSize; j++) {
                    for (int k = 0; k < houseSize; k++) {
                        se.erase(Vec2Int(tmp.x + j, tmp.y + k));
                        se1.erase(Vec2Int(tmp.x + j, tmp.y + k));
                    }
                }
            }
        } else {
            if (se1.empty()) {
                res.push_back(SPECIAL_POINT);
            } else {
                Vec2Int tmp = *se1.begin();
                res.push_back(tmp);
                for (int j = 0; j < otherBaseSize; j++) {
                    for (int k = 0; k < otherBaseSize; k++) {
                        se.erase(Vec2Int(tmp.x + j, tmp.y + k));
                        se1.erase(Vec2Int(tmp.x + j, tmp.y + k));
                    }
                }
            }
        }
    }
    return res;
}

void BitMap::cover(Vec2Int position, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            _bitmap[position.x + i].set(position.y + j);
            _bitmap_fow[position.x + i][position.y + j] = OCCUPIED;
        }
    }
};