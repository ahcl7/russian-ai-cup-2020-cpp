//
// Created by Apple on 14/11/2020.
//

#include "BaseManager.hpp"

BaseManager::BaseManager(const PlayerView &playerView) {
    infoFBM = InformationForBaseManager(playerView);
}

EntityAction BaseManager::getAction(const Entity &b, bool needRangedBase, bool needAttackers) {
    shared_ptr <MoveAction> moveAction = nullptr;
    shared_ptr <BuildAction> buildAction = nullptr;
    shared_ptr <AttackAction> attackAction = nullptr;
    shared_ptr <RepairAction> repairAction = nullptr;
    if (!needRangedBase) {
        if (b.entityType == BUILDER_BASE) {
            if (!needAttackers && this->infoFBM.numberOfBuilders < MAX_NUM_BUILDER_UNIT) {
                buildAction = shared_ptr<BuildAction>(
                        new BuildAction(BUILDER_UNIT, Vec2Int(b.position.x + Utils::getEntitySize(BUILDER_BASE),
                                                              b.position.y + Utils::getEntitySize(BUILDER_BASE) - 1)));
            }
        } else if (b.entityType == RANGED_BASE) {
            buildAction = shared_ptr<BuildAction>(
                    new BuildAction(RANGED_UNIT, Vec2Int(b.position.x + Utils::getEntitySize(RANGED_BASE),
                                                         b.position.y + Utils::getEntitySize(RANGED_BASE) - 1)));
        }
    }
    return EntityAction(moveAction, buildAction, attackAction, repairAction);
}

void BaseManager::update(const PlayerView &playerView) {
    infoFBM.update(playerView);
}
