//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_BITMAP_HPP
#define AICUP_CPP_BITMAP_HPP

#include "Constances.hpp"
#include "model/Model.hpp"
#include "Utils.hpp"
#include <vector>
#include <bitset>
#include <set>
#include <unordered_map>

using namespace std;

enum CellStatus {
    OCCUPIED = 1, // bases, turret, house
    EMPTY = 2,
    UNKNOWN = 3, // haven't know status yet
    EMPTY_OCCUPIED = 4 // empty cells currently occupied by some units (builders, attackers)
};

class BitMap {
public:
    int n;
    bool isFowEnable;
    unordered_map <EntityType, EntityProperties> entityProperties;
    vector <bitset<MAX_SIZE>> bitmap;
    vector <vector<CellStatus>> bitmap_fow;
    vector <bitset<MAX_SIZE>> _bitmap;
    vector <vector<CellStatus>> _bitmap_fow;
    BitMap() {};
    BitMap(int n1);

    void update(const PlayerView &playerView);

    bool canBuild(Vec2Int position, int entitySize);

    void cover(Vec2Int position, int size);

    vector <Vec2Int> getPositionsForNewEntity(EntityType entityType);

    vector <Vec2Int> getBestPositions(vector <EntityType> entityTypes);

    bool isBuilt(Vec2Int position, int entitySize);
};

#endif //AICUP_CPP_BITMAP_HPP
