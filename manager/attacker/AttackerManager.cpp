//
// Created by Apple on 14/11/2020.
//

#include "AttackerManager.hpp"

AttackerManager::AttackerManager(const PlayerView& playerView) {
    infoFAM = InformationForAttackerManager(playerView);
    mcmf = MCMF();
    mcmf1 = MCMF();
    this->update(playerView);
}

EntityAction AttackerManager::getAction(int entityId) {
    AttackerTask currentTask = this->infoFAM.doingTasks[entityId];
    shared_ptr <MoveAction> moveAction = nullptr;
    shared_ptr <BuildAction> buildAction = nullptr;
    shared_ptr <AttackAction> attackAction = nullptr;
    shared_ptr <RepairAction> repairAction = nullptr;

    if (currentTask.attackerTaskType == ATTACK) {
        shared_ptr<int> target = shared_ptr<int>(new int(currentTask.targetId));
        std::vector<EntityType> validAutoAttackTargets;
        shared_ptr<AutoAttack> autoAttack = std::shared_ptr<AutoAttack>(new AutoAttack(10, validAutoAttackTargets));
        attackAction = shared_ptr<AttackAction>(new AttackAction(target, autoAttack));
    } else {
        moveAction = shared_ptr<MoveAction>(new MoveAction(currentTask.targetPosition, true, true));
        shared_ptr<int> target = nullptr;
        std::vector<EntityType> validAutoAttackTargets;
        shared_ptr<AutoAttack> autoAttack = std::shared_ptr<AutoAttack>(new AutoAttack(10, validAutoAttackTargets));
        attackAction = shared_ptr<AttackAction>(new AttackAction(target, autoAttack));
    }
    return EntityAction(moveAction, buildAction, attackAction, repairAction);
}

void AttackerManager::update(const PlayerView& playerView) {
    this->infoFAM.update(playerView);
    this->createTask();
}

Vec2Int getTargetForFirstGroup(set<LANES>& lanes) {
    //TODO: using infoFAM.isDead is cleaner
    if (lanes.count(BOT)) {
        return Utils::getBotCoordinate();
    } else {
        if (lanes.count(TOP)) {
            return Utils::getTopCoordinate();
        } else return Utils::getDiaCoordinate();
    }
}

Vec2Int getTargetForSecondGroup(set<LANES>& lanes) {
    //TODO: using infoFAM.isDead is cleaner
    if (lanes.count(TOP)) {
        return Utils::getTopCoordinate();
    } else {
        if (lanes.count(BOT)) {
            return Utils::getBotCoordinate();
        } else return Utils::getDiaCoordinate();
    }
}

bool AttackerManager::attack(vector<Entity>& attackers, Vec2Int targetPosition) {
//    if (attackers.size() < MINIMUM_NUMBER_OF_ATTACKER) return false;
    vector<Entity> enemyAttackers = this->infoFAM.enemyAttackers;
    int n = attackers.size();
    int m = enemyAttackers.size();
    int s = n + m;
    int t = s + 1;
    mcmf.init(t + 1, s, t);
    mcmf1.init(t + 1, s, t);
    for(int i = 0 ; i < attackers.size(); i++) {
        for(int j = 0 ; j < this->infoFAM.enemyAttackers.size(); j++) {
            if (Utils::distance(attackers[i].position, enemyAttackers[j].position) < Utils::getEntityAttackRange(attackers[i].entityType)) {
                mcmf.add(i, n + j, 1, 0);
            }
            if (Utils::distance(attackers[i].position, enemyAttackers[j].position) < Utils::getEntityAttackRange(enemyAttackers[j].entityType)) {
                mcmf1.add(j, m + i, 1, 0);
            }
            mcmf.add(n + j, t, (enemyAttackers[j].health + 4) / 5, 0);
            mcmf1.add(s, j, 1, 0);
        }
        mcmf.add(s, i, 1, 0);
        mcmf1.add(m + i, t, (attackers[i].health + 4 ) /  5, 0);
    }
    cerr <<"middle" << endl;
    vector<pair<int,int>> vt = mcmf.getPairs(n, m);
    vector<pair<int,int>> vt1 = mcmf1.getPairs(m, n);
    for(auto &a: attackers) {
        this->infoFAM.doingTasks[a.id] = AttackerTask(RUN, -1, targetPosition);
    }
    if (vt.size() >= vt1.size()) {
        //attack
        for(auto& p: vt) {
            int u = p.first;
            int v = p.second;
            this->infoFAM.doingTasks[attackers[u].id] = AttackerTask(ATTACK, enemyAttackers[v].id);
        }
    }
    cerr << "endl" << endl;
    return vt.size() >= vt1.size();
}

bool AttackerManager::getBack(vector<Entity>& attackers) {
    //TODO: improve this strategy
    Vec2Int mediumPoint = Vec2Int(0,0);
    for(auto& a:attackers) {
        Vec2Int newPosition = a.position;
        if (newPosition.x <= newPosition.y) {
            newPosition.y --;
        } else newPosition.x --;
        this->infoFAM.doingTasks[a.id] = AttackerTask(RUN, -1, newPosition);
    }
    for(auto& a:attackers) {
        mediumPoint.x += a.position.x;
        mediumPoint.y += a.position.y;
    }
    if (attackers.size() > 0) {
        mediumPoint.x /= attackers.size();
        mediumPoint.y /= attackers.size();
    }
    for(auto& a:attackers) {
        this->infoFAM.doingTasks[a.id] = AttackerTask(RUN, -1, mediumPoint);
    }
    cerr << "endl1" << endl;
    return false;
}


void AttackerManager::createTask() {
    // attack, defend
    //separate into two groups: attack, defend (maybe three, include scouter)
    // every time new unit born, it will be involved into a specific group
    // at most 1 attack group at a specific time
    // a group can involve a member of other group if it's necessary
    // when fow is enable, should have at least two scouters to scout other player positions
    // target to attack: maybe the weak one first (or is attacking by other player)
    // attackers should know how to hit and run
    // groups should stick together to make a steady structure
    // if in final format, should merge all group into one group
    // should attack when total damage deal is greater than total dame deal by enemy and run otherwise
    // if play size = 2, can claim final format

    // stick
    // prepare force
    // attack
    // defend
//    if (this->infoFAM.started) {
        //find target
        //move units to target base
        //assign for attacker that in enemy range first
        //TODO: merge two groups if only one target remain
        cerr << "force info " << this->infoFAM.firstAttackerGroup.size() <<" " << this->infoFAM.secondAttackerGroup.size() << endl;
        cerr << "position " << getTargetForFirstGroup(this->infoFAM.remainPlayers).x <<" " << getTargetForFirstGroup(this->infoFAM.remainPlayers).y<< endl;
        cerr << "position " << getTargetForSecondGroup(this->infoFAM.remainPlayers).x <<" " << getTargetForSecondGroup(this->infoFAM.remainPlayers).y<< endl;

        this->attack(this->infoFAM.firstAttackerGroup, getTargetForFirstGroup(this->infoFAM.remainPlayers))
            || this->getBack(this->infoFAM.firstAttackerGroup);
        this->attack(this->infoFAM.secondAttackerGroup, getTargetForSecondGroup(this->infoFAM.remainPlayers))
            || this->getBack(this->infoFAM.secondAttackerGroup);
//    } es

}