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
    BitMap(int n1) {
        n = n1;
        for (int i = 0; i < n; i++) {
            bitmap.push_back(bitset<MAX_SIZE>(0));
            bitmap_fow.push_back(vector<CellStatus>(MAX_SIZE, UNKNOWN));
        }
        _bitmap = bitmap;
        _bitmap_fow = bitmap_fow;
    }

    void update(const PlayerView &playerView);

    bool canBuild(Vec2Int position, int entitySize);

    void cover(Vec2Int position, int size);

    vector <Vec2Int> getPositionsForNewEntity(EntityType entityType);

    vector <Vec2Int> getBestPositions(vector <EntityType> entityTypes);

    bool isBuilt(Vec2Int position, int entitySize);
};

#endif //AICUP_CPP_BITMAP_HPP
