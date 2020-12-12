#include "MyStrategy.hpp"
#include <exception>
#include <iostream>
#include <map>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <set>
#include <bitset>

using namespace std;
#define TOP 1
#define BOT 2
#define DIA 3

enum BUILDER_STATUS {
    WAITING = 1,
    REPAIRING = 2,
    BUILDING = 3,
    MINING = 4
};



MyStrategy::MyStrategy() {}

const int SCOUT_TIME = 50;

const int INF = 1e5;

const int DEFEND_RANGE = 25;

const int DEATH_NUM = 4;

const int BASE_CENTER = 10;

const double RATE = 4;

const int REPAIRER_INVOLVE_NUMBER = 3;

const int POPULATION_BACKUP_QUANTITY = 5;

const int MAXIMUM_NUMBER_DEFENDER = 15;

const int MAX_NUM_BUILDER_UNIT = 50;

const int MIN_BUILDERS = 5;

map<int, int> playersPosition;

//validated
int distance(Vec2Int a, Vec2Int b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}
//validated
int distance(Entity a, Entity b) {
    return distance(a.position, b.position);
}
//validated
int sqr(int x) {
    return x * x;
}
//validated
int distance2(Vec2Int a, Vec2Int b) {
    return sqr(a.x - b.x) + sqr(a.y - b.y);
}
//validated
bool intersect(int l, int r, int u, int v) {
    int L = std::max(l, u);
    int R = std::min(r, v);
    return (L <= R);
}
//validated
bool intersect(Vec2Int a, int sa, Vec2Int b, int sb) {
    bool res = intersect(a.x, a.x + sa - 1, b.x, b.x + sb - 1);
    res &= intersect(a.y, a.y + sa - 1, b.y, b.y + sb - 1);
    return res;
}
//validated

vector <Vec2Int> getPossiblePositions(const PlayerView &playerView, EntityType entityType) {
    int mapSize = playerView.mapSize;
    int entitySize = playerView.entityProperties.at(entityType).size;
    vector<bitset<150>> ma(mapSize + 10);
    for(int i = 0 ; i < playerView.entities.size(); i++) {
        const Entity& entity = playerView.entities[i];
        EntityType entityType = entity.entityType;
        int sz = playerView.entityProperties.at(entityType).size;
        Vec2Int pos = entity.position;
        for(int j = 0; j < sz; j++) {
            for(int k = 0; k < sz; k++) {
                ma[pos.x + j].set(pos.y + k);
            }
        }
    }
    vector <Vec2Int> res;
    for (int i = 1; i < mapSize - entitySize; i++) {
        for (int j = 1; j < mapSize - entitySize; j++) {
            bool ok = true;
            for(int k = 0; k < entitySize && ok; k++) {
                for(int l = 0; l < entitySize && ok; l++) {
                    if (ma[i + k].test(j + l)) ok = false;
                }
            }
            if (ok) {
                res.push_back(Vec2Int(i, j));
            }
        }
    }
    return res;
}

bool isAdjacent(Vec2Int a, Vec2Int b, int sz) {
    for (int i = 0; i < sz; i++) {
        for (int j = 0; j < sz; j++) {
            int x = b.x + i;
            int y = b.y + j;
            if (abs(a.x - x) + abs(a.y - y) == 1) return true;
        }
    }
    return false;
}

int mapSize;


Vec2Int getTopCoordinate() {
    return Vec2Int(BASE_CENTER, mapSize - BASE_CENTER);
}

Vec2Int getBotCoordinate() {
    return Vec2Int(mapSize - BASE_CENTER, BASE_CENTER);
}

Vec2Int getDiaCoordinate() {
    return Vec2Int(mapSize - BASE_CENTER, mapSize - BASE_CENTER);
}

Vec2Int getBaseCoordinate() {
    return Vec2Int(BASE_CENTER, BASE_CENTER);
}

Vec2Int getCenterCoordinate() {
    return Vec2Int(mapSize / 2, mapSize / 2);
}

Vec2Int getCoordinateByLane(int p) {
    if (p == TOP) return getTopCoordinate();
    if (p == BOT) return getBotCoordinate();
    if (p == DIA) return getDiaCoordinate();
    return getCenterCoordinate();
}

int getUnderAttackRange() {
    int t = mapSize / 2 - BASE_CENTER;
    return t;
}

int guessPlayerPosition(Vec2Int p) {
    int dis = distance(p, getTopCoordinate());
    int dis1 = distance(p, getBotCoordinate());
    int dis2 = distance(p, getDiaCoordinate());
    if (dis <= dis1) {
        if (dis <= dis2) return TOP;
        else return DIA;
    } else if (dis1 <= dis2) return BOT;
    else return DIA;
}

void guessPlayerPosition(const PlayerView &playerView) {
    int myId = playerView.myId;
    for (int j = 0; j < playerView.players.size(); j++) {
        if (playerView.players[j].id == myId) continue;
        int cnt = 0;
        Vec2Int mediumPoint = Vec2Int(0, 0);
        for (int i = 0; i < playerView.entities.size(); i++) {
            if (playerView.entities[i].playerId != nullptr && *playerView.entities[i].playerId == playerView.players[j].id) {
                cnt ++;
                mediumPoint.x += playerView.entities[i].position.x;
                mediumPoint.y += playerView.entities[i].position.y;
            }
        }
        if (cnt > 0) {
            mediumPoint.x /= cnt;
            mediumPoint.y /= cnt;
            playersPosition[playerView.players[j].id] = guessPlayerPosition(mediumPoint);
            cerr << "Medium point " << mediumPoint.x <<" " << mediumPoint.y << endl;
            cerr << "GUESS " << playerView.players[j].id <<" " << playersPosition[playerView.players[j].id];
        }
    }
}

int getPlayerIdAt(int lane) {
    for(auto& p:playersPosition) {
        if (p.second == lane) return p.first;
    }
    return -1;
}

bool death(const PlayerView& playerView, int playerId) {
    int cnt = 0;
    for(int i = 0 ; i < playerView.entities.size(); i++) {
        if (playerView.entities[i].playerId == nullptr || *playerView.entities[i].playerId != playerId) continue;
        int entityType = playerView.entities[i].entityType;
        if (entityType == BUILDER_BASE || entityType == RANGED_BASE || entityType == MELEE_BASE) {
            return false;
        }
        else if (entityType == BUILDER_UNIT || entityType == RANGED_UNIT || entityType == MELEE_UNIT) cnt++;
    }
    cerr << "REMAIN UNITS: " << cnt << endl;
    return cnt < DEATH_NUM;
}

Vec2Int getTarget(const PlayerView& playerView) {
    if (getPlayerIdAt(BOT) != -1 && !death(playerView, getPlayerIdAt(BOT))) return getBotCoordinate();
    cerr << "BOT DEAD" << endl;
    if (getPlayerIdAt(TOP) != -1 && !death(playerView, getPlayerIdAt(TOP))) return getTopCoordinate();
    cerr << "TOP DEAD" << endl;
    if (getPlayerIdAt(DIA) != -1 && !death(playerView, getPlayerIdAt(DIA))) return getDiaCoordinate();
    cerr << "DIA DEAD" << endl;
    return getCenterCoordinate();
}

set<int> builderInvolved;
pair<int, Vec2Int> getOptimizePosition(const PlayerView& playerView, EntityType entityType) {
    int myId = playerView.myId;
    clock_t tStart = clock();
    vector <Vec2Int> positions = getPossiblePositions(playerView, entityType);
    int minDis = playerView.mapSize * 2;

    int builderId = -1;
    Vec2Int basePosition = Vec2Int(0, 0);
    for (int i = 0; i < positions.size(); i++) {
        for (int j = 0; j < playerView.entities.size(); j++) {
            const Entity &entity = playerView.entities[j];
            if (entity.playerId == nullptr || *entity.playerId != myId && entity.entityType != BUILDER_UNIT ||
                builderInvolved.count(entity.id))
                continue;
            if (distance(entity.position, positions[i]) < minDis) {
                minDis = distance(entity.position, positions[i]);
                builderId = entity.id;
                basePosition = positions[i];
            }
        }
    }


    clock_t tEnd = clock();
    std::cerr << playerView.mapSize * playerView.mapSize * playerView.entities.size() << " " << "Execution time "
              << double(tEnd - tStart) / CLOCKS_PER_SEC << " ms" << std::endl;
    cerr << "mapSize " << playerView.mapSize << endl;

    return make_pair(builderId, basePosition);
}

Action MyStrategy::getAction(const PlayerView &playerView, DebugInterface *debugInterface) {
    builderInvolved.clear();
    mapSize = playerView.mapSize;
    Action result = Action(std::unordered_map<int, EntityAction>());
    int myId = playerView.myId;
    int resource = 0;
    std::map<int, int> repairer;
    std::map<int, int> count;
    std::map<int, Entity> bases;
    vector <Entity> myUnits;
    vector <Entity> myAttackUnits;
    for (int i = 0; i < playerView.entities.size(); i++) {
        const Entity &entity = playerView.entities[i];
        if (entity.playerId == nullptr || *entity.playerId != myId ||
            !playerView.entityProperties.at(entity.entityType).canMove)
            continue;
        myUnits.push_back(entity);
        if (entity.entityType != BUILDER_UNIT) {
            myAttackUnits.push_back(entity);
        }
    }

    bool isUnderAttacked = false;

    set<int> defenders;
    Vec2Int mediumPoint = Vec2Int(0, 0);
    int attackCnt = 0;
    for (int i = 0; i < playerView.entities.size(); i++) {
        if (playerView.entities[i].playerId != nullptr && *playerView.entities[i].playerId != myId &&
            distance2(playerView.entities[i].position, getBaseCoordinate()) < sqr(getUnderAttackRange())) {
            isUnderAttacked = true;
            mediumPoint.x += playerView.entities[i].position.x;
            mediumPoint.y += playerView.entities[i].position.y;
            attackCnt ++;
        }
    }
    if (attackCnt > 0) {
        mediumPoint.x /= attackCnt;
        mediumPoint.y /= attackCnt;
    }

    if (isUnderAttacked) {
        cerr << "########--------UNDER ATTACK-------######" << endl;
        for (int i = 0; i < myAttackUnits.size(); i++) {
            if (distance(myAttackUnits[i].position, mediumPoint) < DEFEND_RANGE) {
                defenders.insert(myAttackUnits[i].id);
            }
        }
        cerr << "INVOLVED " << defenders.size() <<" DEFENDERS" << endl;
    }


    if (playersPosition.empty()) {
        guessPlayerPosition(playerView);
    }

    Vec2Int targetPosition = getTarget(playerView);
    cerr << "TARGET POSITION" << targetPosition.x <<" " <<targetPosition.y << endl;
    for (size_t i = 0; i < playerView.entities.size(); i++) {
        const Entity &entity = playerView.entities[i];
        if (entity.playerId == nullptr || *entity.playerId != myId) {
            continue;
        }

        count[entity.entityType]++;
        bases[entity.id] = entity;
        if (playerView.entityProperties.at(entity.entityType).canMove) continue;
        if (entity.health < playerView.entityProperties.at(entity.entityType).maxHealth) {
            std::vector<std::pair<int, int>> vt;
            for (size_t j = 0; j < playerView.entities.size(); j++) {
                const Entity &entity1 = playerView.entities[j];
                if (entity1.playerId == nullptr || *entity1.playerId != myId || entity1.entityType != BUILDER_UNIT ||
                    repairer.count(entity1.id)) {
                    continue;
                }
                vt.push_back(std::make_pair(distance(entity, entity1), entity1.id));
            }

            sort(vt.begin(), vt.end());
            int need = (playerView.entityProperties.at(entity.entityType).maxHealth - entity.health + 4) / 5;
            need = min(need, REPAIRER_INVOLVE_NUMBER);
            need = min(need, (int) vt.size());

            cerr << "INVOLVED " << need << "REPAIRERS" << endl;
            for (int k = 0; k < need; k++) {
                repairer[vt[k].second] = entity.id;
                builderInvolved.insert(vt[k].second);
            }
        }
    }
    int totalPopulationProvide = (count[HOUSE] + count[BUILDER_BASE] + count[RANGED_BASE] + count[MELEE_BASE]) * 5;
    int curPopulation = count[RANGED_UNIT] + count[MELEE_UNIT] + count[BUILDER_UNIT];
    for (size_t i = 0; i < playerView.players.size(); i++) {
        if (playerView.players[i].id == myId) {
            resource = playerView.players[i].resource;
        }
    }
    std::cerr << totalPopulationProvide << " " << curPopulation << " " << resource << std::endl;

    bool needMoreHouse = (curPopulation + POPULATION_BACKUP_QUANTITY >= totalPopulationProvide && count[BUILDER_UNIT] >= MIN_BUILDERS);
    bool enoughResourceForHouse = playerView.entityProperties.at(HOUSE).initialCost <= resource;

    map<int, pair<EntityType, Vec2Int>> baseBuildingTasks;
    int houseBuilder = -1;
    Vec2Int housePosition;
    if (needMoreHouse && enoughResourceForHouse) {
        pair<int, Vec2Int> t = getOptimizePosition(playerView, HOUSE);
        if (t.first != -1) {
            baseBuildingTasks[t.first] = make_pair(HOUSE, t.second);
            resource -= playerView.entityProperties.at(HOUSE).initialCost;
        }
    }

    bool needBuilderBase = (count[BUILDER_BASE] == 0 && playerView.entityProperties.at(BUILDER_BASE).initialCost <= resource);

    if (needBuilderBase) {
        cerr << "NEED BUILDER BASE" << endl;
        pair<int, Vec2Int> t = getOptimizePosition(playerView, HOUSE);
        if (t.first != -1) {
            baseBuildingTasks[t.first] = make_pair(BUILDER_BASE, t.second);
            builderInvolved.insert(t.first);
            resource -= playerView.entityProperties.at(BUILDER_BASE).initialCost;
        }
    }

    bool needRangedBase = (!needBuilderBase && ((count[RANGED_BASE] == 0 &&
            playerView.entityProperties.at(RANGED_BASE).initialCost <= resource) ||
            (resource / playerView.entityProperties.at(RANGED_BASE).initialCost) >= RATE));
    if (needRangedBase) {
        cerr << "NEED RANGED BASE" << endl;
        pair<int, Vec2Int> t = getOptimizePosition(playerView, RANGED_BASE);
        if (t.first != -1) {
            baseBuildingTasks[t.first] = make_pair(RANGED_BASE, t.second);
            builderInvolved.insert(t.first);
            resource -= playerView.entityProperties.at(RANGED_BASE).initialCost;

        }
    }

    bool needMeleeBase = (!needBuilderBase && !needRangedBase && count[MELEE_BASE] == 0 &&
            playerView.entityProperties.at(MELEE_BASE).initialCost <= resource);
    if (needMeleeBase) {
        cerr << "NEED MELEE BASE" << endl;
        pair<int, Vec2Int> t = getOptimizePosition(playerView, MELEE_BASE);
        if (t.first != -1) {
            baseBuildingTasks[t.first] = make_pair(MELEE_BASE, t.second);
            builderInvolved.insert(t.first);
            resource -= playerView.entityProperties.at(MELEE_BASE).initialCost;

        }
    }

    for (size_t i = 0; i < playerView.entities.size(); i++) {
        const Entity &entity = playerView.entities[i];

        if (entity.playerId == nullptr || *entity.playerId != myId) {
            continue;
        }
        const EntityProperties &properties = playerView.entityProperties.at(entity.entityType);
        std::shared_ptr<MoveAction> moveAction = nullptr;
        std::shared_ptr<BuildAction> buildAction = nullptr;
        std::shared_ptr<RepairAction> repairAction = nullptr;

        if (properties.canMove) {
            // melee, ranged, builder units
            if (entity.entityType == BUILDER_UNIT) {

                if (repairer.count(entity.id)) {
                    int baseId = repairer[entity.id];
                    int baseSize = playerView.entityProperties.at(bases[baseId].entityType).size;
                    Vec2Int basePosition = bases[baseId].position;
                    if (isAdjacent(entity.position, basePosition, baseSize))
                        repairAction = std::shared_ptr<RepairAction>(new RepairAction(repairer[entity.id]));
                    else
                        moveAction = std::shared_ptr<MoveAction>(new MoveAction(
                                Vec2Int(basePosition.x + baseSize / 2, basePosition.y + baseSize / 2),
                                true,
                                true));
                } else if (baseBuildingTasks.count(entity.id)) {
                    EntityType type = baseBuildingTasks[entity.id].first;
                    Vec2Int position = baseBuildingTasks[entity.id].second;
                    buildAction = std::shared_ptr<BuildAction>(new BuildAction(type,
                                                                               position));
                    std::cerr << "BUILD " << housePosition.x << " " << housePosition.y << std::endl;
                    std::cerr << "CurPos" << entity.position.x << " " << entity.position.y << std::endl;
                    totalPopulationProvide += 5;
                } else {
                    moveAction = std::shared_ptr<MoveAction>(new MoveAction(
                            Vec2Int(playerView.mapSize / 3, playerView.mapSize - 1),
                            true,
                            true));
                }
            } else {
                if (defenders.count(entity.id)) {
                    moveAction = std::shared_ptr<MoveAction>(new MoveAction(
                            mediumPoint, true, true
                    ));
                } else {
                    moveAction = std::shared_ptr<MoveAction>(new MoveAction(
                            targetPosition,
                            true,
                            true));
                }
            }
        } else if (properties.build != nullptr && totalPopulationProvide > curPopulation) {
            EntityType entityType = HOUSE;

            if (!needMoreHouse || enoughResourceForHouse) {
                if (entity.entityType == BUILDER_BASE) {
                    if (count[BUILDER_UNIT] < MAX_NUM_BUILDER_UNIT ||
                        count[BUILDER_UNIT] < count[RANGED_UNIT] + count[MELEE_UNIT]) {
                        if (playerView.entityProperties.at(BUILDER_UNIT).initialCost + count[BUILDER_UNIT] <=
                            resource) {
                            entityType = BUILDER_UNIT;
                        }
                    }
                } else {
                    if (entity.entityType == RANGED_BASE &&
                        playerView.entityProperties.at(RANGED_UNIT).initialCost + count[RANGED_UNIT] <= resource) {
                        entityType = RANGED_UNIT;
                    }
                    if (entity.entityType == MELEE_BASE &&
                        playerView.entityProperties.at(MELEE_UNIT).initialCost + count[MELEE_UNIT] <= resource) {
                        entityType = MELEE_UNIT;
                    }
                }
            }
            if (entityType != HOUSE) {
                buildAction = std::shared_ptr<BuildAction>(new BuildAction(entityType,
                                                                           Vec2Int(entity.position.x + properties.size,
                                                                                   entity.position.y + properties.size -
                                                                                   1)));
                resource -= playerView.entityProperties.at(entityType).initialCost + count[entityType];
            }
        }
        std::vector<EntityType> validAutoAttackTargets;
        if (entity.entityType == BUILDER_UNIT && buildAction == nullptr && repairAction == nullptr) {
            validAutoAttackTargets.push_back(RESOURCE);
        }
        result.entityActions[entity.id] = EntityAction(
                moveAction,
                buildAction,
                std::shared_ptr<AttackAction>(new AttackAction(
                        nullptr,
                        std::shared_ptr<AutoAttack>(new AutoAttack(properties.sightRange, validAutoAttackTargets)))),
                repairAction);
    }
    return result;
}

void MyStrategy::debugUpdate(const PlayerView &playerView, DebugInterface &debugInterface) {
    debugInterface.send(DebugCommand::Clear());
    debugInterface.getState();
}
