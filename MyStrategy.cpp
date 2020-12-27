#include "MyStrategy.hpp"


using namespace std;
#define TOP 1
#define BOT 2
#define DIA 3

MyStrategy::MyStrategy() {}

namespace Utils {
    int mapSize = 0;
    int myId = -1;
    unordered_map<EntityType, EntityProperties> entityProperties;
}

Manager manager;

bool isManagerInitialized = false;

Action MyStrategy::getAction(const PlayerView &playerView, DebugInterface *debugInterface) {
    Action result = Action(std::unordered_map<int, EntityAction>());
    if (Utils::mapSize == 0) {
        Utils::mapSize = playerView.mapSize;
    }
    if (Utils::entityProperties.empty()) Utils::entityProperties = playerView.entityProperties;

    Utils::myId = playerView.myId;
    if (!isManagerInitialized) {
        manager = Manager(playerView);
        isManagerInitialized = true;
    } else manager.update(playerView);

    for (size_t i = 0; i < playerView.entities.size(); i++) {
        const Entity &entity = playerView.entities[i];
        if (!Utils::isMyEntity(entity)) {
            continue;
        }
        std::shared_ptr<MoveAction> moveAction = nullptr;
        std::shared_ptr<BuildAction> buildAction = nullptr;
        std::shared_ptr<RepairAction> repairAction = nullptr;
        std::shared_ptr<AttackAction> attackAction = nullptr;
        EntityAction entityAction = EntityAction(moveAction, buildAction, attackAction, repairAction);
        if (Utils::getEntityProperties(entity.entityType).canMove) {
            // melee, ranged, builder units
            if (entity.entityType == BUILDER_UNIT) {
                entityAction = manager.getActionForBuilder(entity.id);
                result.entityActions[entity.id] = entityAction;
            } else if (entity.entityType == RANGED_UNIT) {
                entityAction = manager.getActionForAttacker(entity.id);
                result.entityActions[entity.id] = entityAction;
            }
        } else {
            if (entity.entityType == BUILDER_BASE || entity.entityType == RANGED_BASE) {
                entityAction = manager.getActionForBase(entity);
                result.entityActions[entity.id] = entityAction;
            }
        }
    }

    return result;
}

void MyStrategy::debugUpdate(const PlayerView &playerView, DebugInterface &debugInterface) {
    debugInterface.send(DebugCommand::Clear());
    debugInterface.getState();
}
