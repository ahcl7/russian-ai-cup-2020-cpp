//
// Created by Apple on 14/11/2020.
//

#include "AttackerManager.hpp"

AttackerManager::AttackerManager(const PlayerView& playerView) {
    infoFAM = InformationForAttackerManager(playerView);
    this.update(playerView);
}

EntityAction AttackerManager::getAction(int entityId) {

}

void AttackerManager::update(const PlayerView& playerView) {

}