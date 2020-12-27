//
// Created by Apple on 14/11/2020.
//

#include "AttackerManager.hpp"

unordered_map<int, int> attackerIndexMap;
unordered_map<int, int> enemyAttackerIndexMap;
unordered_map<int, int> _attackerIndexMap;
unordered_map<int, int> _enemyAttackerIndexMap;
unordered_map<Vec2Int, int> enemyPositionIdMap;

AttackerManager::AttackerManager(const PlayerView &playerView) {
    infoFAM = InformationForAttackerManager(playerView);
    mcmf = MCMF();
    mcmf1 = MCMF();
    this->update(playerView);
}

EntityAction AttackerManager::getAction(int entityId) {
    AttackerTask currentTask = this->infoFAM.doingTasks[entityId];
    shared_ptr<MoveAction> moveAction = nullptr;
    shared_ptr<BuildAction> buildAction = nullptr;
    shared_ptr<AttackAction> attackAction = nullptr;
    shared_ptr<RepairAction> repairAction = nullptr;

    if (currentTask.attackerTaskType == ATTACK) {
        shared_ptr<int> target = shared_ptr<int>(new int(currentTask.targetId));
        std::vector<EntityType> validAutoAttackTargets;
        shared_ptr<AutoAttack> autoAttack = std::shared_ptr<AutoAttack>(new AutoAttack(10, validAutoAttackTargets));
        attackAction = shared_ptr<AttackAction>(new AttackAction(target, autoAttack));
    } else if (currentTask.attackerTaskType == MOVE) {
        moveAction = shared_ptr<MoveAction>(new MoveAction(currentTask.targetPosition, true, true));
        shared_ptr<int> target = nullptr;
        std::vector<EntityType> validAutoAttackTargets;
        shared_ptr<AutoAttack> autoAttack = std::shared_ptr<AutoAttack>(new AutoAttack(10, validAutoAttackTargets));
        attackAction = shared_ptr<AttackAction>(new AttackAction(target, autoAttack));
    } else {
        moveAction = shared_ptr<MoveAction>(new MoveAction(currentTask.targetPosition, true, true));
    }
    return EntityAction(moveAction, buildAction, attackAction, repairAction);
}

void AttackerManager::update(const PlayerView &playerView) {
    this->infoFAM.update(playerView);
    this->createTask();
}

Vec2Int getTargetForFirstGroup(set<LANES> &lanes) {
    //TODO: using infoFAM.isDead is cleaner
    if (lanes.count(BOT)) {
        return Utils::getBotCoordinate();
    } else {
        if (lanes.count(TOP)) {
            return Utils::getTopCoordinate();
        } else return Utils::getDiaCoordinate();
    }
}

Vec2Int getTargetForSecondGroup(set<LANES> &lanes) {
    //TODO: using infoFAM.isDead is cleaner
    if (lanes.count(TOP)) {
        return Utils::getTopCoordinate();
    } else {
        if (lanes.count(BOT)) {
            return Utils::getBotCoordinate();
        } else return Utils::getDiaCoordinate();
    }
}

vector<Entity> AttackerManager::attack(vector<Entity> &attackers, Vec2Int targetPosition) { //return getback list
//    if (attackers.size() < MINIMUM_NUMBER_OF_ATTACKER) return false;

    // defend if under attack
    vector<Entity> enemyAttackerAttackingMe;
    for (auto &e: this->infoFAM.enemyAttackers) {
        if (Utils::distance2(e.position, Utils::getBaseCoordinate()) <= Utils::sqr(Utils::mapSize / 2 - 10)) {
            enemyAttackerAttackingMe.push_back(e);
        }
    }
    vector<Entity> defendList;
    vector<Entity> attackList;
    Vec2Int P = Utils::getMediumPoint(enemyAttackerAttackingMe);
    if (enemyAttackerAttackingMe.size() > 0) {
        for (auto &a:attackers) {
            if (Utils::distance2(a.position, P) < Utils::sqr(Utils::mapSize / 2 - 10)) {
                defendList.push_back(a);
            } else attackList.push_back(a);
        }
    } else attackList = attackers;
    vector<Entity> enemyAttackers = this->infoFAM.enemyAttackers;
    enemyPositionIdMap.clear();
    for(int i = 0 ; i < enemyAttackers.size(); i++) {
        enemyPositionIdMap[enemyAttackers[i].position] = i;
    }
    // TODO: replace all map to unordered_map

    int n = 0;
    int m = 0;

    _attackerIndexMap.clear();
    _enemyAttackerIndexMap.clear();
    attackerIndexMap.clear();
    enemyAttackerIndexMap.clear();
    vector<pair<int,int>> pairs;
    for(int i = 0 ; i < attackList.size(); i++) {
        int attackRange = Utils::getEntityAttackRange(attackList[i].entityType);
        for(int j = -attackRange; j <= attackRange; j++) {
            for(int k = -attackRange; k <= attackRange; k++) {
                int t = abs(j) + abs(k);
                if (t <= attackRange) {
                    Vec2Int p = Vec2Int(attackList[i].position.x + j, attackList[i].position.y + k);
                    if (enemyPositionIdMap.count(p)) {
                        if (!attackerIndexMap.count(i)) {
                            _attackerIndexMap[n] = i;
                            attackerIndexMap[i] = n++;
                        }
                        int idx = enemyPositionIdMap[p];
                        if (!enemyAttackerIndexMap.count(idx)) {
                            _enemyAttackerIndexMap[m] = idx;
                            enemyAttackerIndexMap[idx] = m ++;
                        }
                        pairs.push_back(make_pair(attackerIndexMap[i], enemyAttackerIndexMap[idx]));
                    }
                }
            }
        }
    }
    cerr << "cnt " << n + m << endl;
    int s = n + m;
    int t = s + 1;
    mcmf.init(t + 1, s, t);
    for(auto &[first, second]: pairs) {
        mcmf.add(first, n + second, 1, 0);
    }
    for(int i = 0 ; i < n; i++) {
        mcmf.add(s, i, 1, 0);
    }
    for(int j = 0 ; j < m; j++) {
        int u = _enemyAttackerIndexMap[j];
        mcmf.add(n + j, t, min(10, (enemyAttackers[u].health + 4) / 5),
                 min(10, (enemyAttackers[u].health + 4) / 5));
    }
    vector<pair<int, int>> vt = mcmf.getPairs(n, m);
    if (attackList.size() < MINIMUM_NUMBER_OF_ATTACKER_TO_ATTACK) {
        if (targetPosition.x == Utils::mapSize - BASE_CENTER) {
            targetPosition.x = Utils::mapSize / 2 - BASE_CENTER;
        }
        if (targetPosition.y == Utils::mapSize - BASE_CENTER) {
            targetPosition.y = Utils::mapSize / 2 - BASE_CENTER;
        }
    }
    for (auto &a: attackList) {
        this->infoFAM.doingTasks[a.id] = AttackerTask(MOVE, -1, targetPosition);
    }
    for (auto &d: defendList) {
        this->infoFAM.doingTasks[d.id] = AttackerTask(MOVE, -1, P); // move defender to medium point
    }

    vector<Entity> getBackList;
    for (auto &[u,v]: vt) {
        u = _attackerIndexMap[u];
        v = _enemyAttackerIndexMap[v];
        this->infoFAM.doingTasks[attackList[u].id] = AttackerTask(ATTACK, enemyAttackers[v].id);
    }
    return getBackList;
}

bool AttackerManager::getBack(vector<Entity> attackers) {

    // getBack is not supported yet
    //TODO: improve this strategy
    return true;
    Vec2Int P = Utils::getMediumPoint(attackers);
    for (auto &a:attackers) {
        bool isUnderAttack = false;
        for (auto &e:this->infoFAM.enemyAttackers) {
            if (Utils::distance(a.position, e.position) <= Utils::getEntityAttackRange(e.entityType)) {
                isUnderAttack = true;
            }
        }
        if (isUnderAttack) {
//            Vec2Int newPosition = a.position;
//            if (newPosition.x >= newPosition.y) newPosition.x--;
//            else newPosition.y--;
            this->infoFAM.doingTasks[a.id] = AttackerTask(RUN, -1, P);
        }
    }
    return false;
}


void AttackerManager::createTask() {
    this->getBack(this->attack(this->infoFAM.firstAttackerGroup, getTargetForFirstGroup(this->infoFAM.remainPlayers)));
    this->getBack(this->attack(this->infoFAM.secondAttackerGroup, getTargetForFirstGroup(this->infoFAM.remainPlayers)));
}