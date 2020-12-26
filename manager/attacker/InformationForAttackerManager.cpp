//
// Created by Apple on 14/11/2020.
//

#include "InformationForAttackerManager.hpp"

InformationForAttackerManager::InformationForAttackerManager(const PlayerView& playerView) {
    started = false;
    if (playerView.players.size() == 2) {
        this->remainPlayers.insert(DIA);
    } else {
        this->remainPlayers.insert(TOP);
        this->remainPlayers.insert(BOT);
        this->remainPlayers.insert(DIA);
    }
}

bool InformationForAttackerManager::isDead(LANES lane) {
    if (!remainPlayers.count(lane)) return true;
    Vec2Int position = (lane == TOP ? Utils::getTopCoordinate() : (lane == BOT ? Utils::getBotCoordinate() : Utils::getDiaCoordinate()));
    //check if any my entity is near position, and number of house & base is == 0
    bool ok = false;
    for(auto& a: attackers) {
        if (Utils::distance(a.position, position) < DISTANCE_ASSUME_TO_DEATH) {
            ok = true;
        }
    }
    if (!ok) return false;
    // count number of house in around position area
    for(auto& h:enemyHouseAndBases) {
        if (Utils::distance(h.position, position) < BASE_RANGE) { //has house near target position
            return false;
        }
    }
//    exit(0);
    return true;
}

void InformationForAttackerManager::update(const PlayerView &playerView) {
    curTick = playerView.currentTick;
    numberOfPlayers = playerView.players.size();
    vector<Entity> newAttackers;
    map <int, Entity> newAttackerMap;
    enemyAttackers.clear();
    enemyBuilders.clear();
    enemyHouseAndBases.clear();
    numberOfBuilders = 0;
    numberOfRangedBase = 0;
    for(int i = 0 ; i < playerView.entities.size(); i++) {
        const Entity&  entity = playerView.entities[i];
        EntityType entityType = entity.entityType;
        if (Utils::isMyEntity(entity)) {
            //TODO: may be accept only ranged unit
            if (entityType == RANGED_UNIT || entityType == MELEE_UNIT) {
                newAttackers.push_back(entity);
                newAttackerMap[entity.id] = entity;
            }
            if (entityType == BUILDER_UNIT) {
                numberOfBuilders++;
            }
            if (entityType == RANGED_BASE) {
                numberOfRangedBase++;
            }
        }
        if (!Utils::isMyEntity(entity)) {
            if (entityType == RANGED_UNIT || entityType == MELEE_UNIT || entityType == TURRET)  {
                enemyAttackers.push_back(entity);
            } else
            if (entityType == BUILDER_UNIT) {
                enemyBuilders.push_back(entity);
            } else
            if (entityType != RESOURCE) {
                enemyHouseAndBases.push_back(entity);
            }
        }
    }
    vector<Entity> newFirstAttackerGroup;
    for(auto&p: firstAttackerGroup) {
        if (newAttackerMap.count(p.id)) {
            newFirstAttackerGroup.push_back(newAttackerMap[p.id]); //keep alive attackers
        }
    }
    vector<Entity> newSecondAttackerGroup;
    for(auto&p: secondAttackerGroup) {
        if (newAttackerMap.count(p.id)) {
            newSecondAttackerGroup.push_back(newAttackerMap[p.id]); //keep alive attackers
        }
    }
    for(auto& p:newAttackers) {
        if (!attackerMap.count(p.id)) { // new attacker
            if (newFirstAttackerGroup.size() <= newSecondAttackerGroup.size()) {
                newFirstAttackerGroup.push_back(p);
            } else newSecondAttackerGroup.push_back(p);
        }
    }
    attackers = newAttackers;
    firstAttackerGroup = newFirstAttackerGroup;
    secondAttackerGroup = newSecondAttackerGroup;
    attackerMap = newAttackerMap;

    needAttackers = numberOfRangedBase > 0 && attackers.size() < MINIMUM_NUMBER_OF_ATTACKER && numberOfBuilders >= 8;
    //_TODO: update remain players
    if (remainPlayers.count(TOP) && isDead(TOP)) remainPlayers.erase(TOP);
    if (remainPlayers.count(BOT) && isDead(BOT)) remainPlayers.erase(BOT);
    if (remainPlayers.count(DIA) && isDead(DIA)) remainPlayers.erase(DIA);

}