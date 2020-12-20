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
            }
        } else {
            if (entity.entityType == BUILDER_BASE) {
                buildAction = std::shared_ptr<BuildAction>(new BuildAction(BUILDER_UNIT,
                                                                           Vec2Int(entity.position.x + Utils::getEntitySize(entity.entityType),
                                                                                   entity.position.y + Utils::getEntitySize(entity.entityType) - 1)));
                entityAction = EntityAction(moveAction, buildAction, attackAction, repairAction);
                result.entityActions[entity.id] = entityAction;
            }
        }
    }
//                if (repairer.count(entity.id)) {
//                    int baseId = repairer[entity.id];
//                    int baseSize = playerView.entityProperties.at(bases[baseId].entityType).size;
//                    Vec2Int basePosition = bases[baseId].position;
//                    if (isAdjacent(entity.position, basePosition, baseSize))
//                        repairAction = std::shared_ptr<RepairAction>(new RepairAction(repairer[entity.id]));
//                    else
//                        moveAction = std::shared_ptr<MoveAction>(new MoveAction(
//                                Vec2Int(basePosition.x + baseSize / 2, basePosition.y + baseSize / 2),
//                                true,
//                                true));
//                } else if (baseBuildingTasks.count(entity.id)) {
//                    EntityType type = baseBuildingTasks[entity.id].first;
//                    Vec2Int position = baseBuildingTasks[entity.id].second;
//                    buildAction = std::shared_ptr<BuildAction>(new BuildAction(type,
//                                                                               position));
//                    std::cerr << "BUILD " << housePosition.x << " " << housePosition.y << std::endl;
//                    std::cerr << "CurPos" << entity.position.x << " " << entity.position.y << std::endl;
//                    totalPopulationProvide += 5;
//                } else {
//                    moveAction = std::shared_ptr<MoveAction>(new MoveAction(
//                            Vec2Int(playerView.mapSize / 3, playerView.mapSize - 1),
//                            true,
//                            true));
//                }

            //TODO: remove comment
//            } else {
//                if (defenders.count(entity.id)) {
//                    moveAction = std::shared_ptr<MoveAction>(new MoveAction(
//                            mediumPoint, true, true
//                    ));
//                } else {
//                    moveAction = std::shared_ptr<MoveAction>(new MoveAction(
//                            targetPosition,
//                            true,
//                            true));
//                }
//            }
//        } else if (properties.build != nullptr && totalPopulationProvide > curPopulation) {
//            EntityType entityType = HOUSE;
//
//            if (!needMoreHouse || enoughResourceForHouse) {
//                if (entity.entityType == BUILDER_BASE) {
//                    if (count[BUILDER_UNIT] < MAX_NUM_BUILDER_UNIT ||
//                        count[BUILDER_UNIT] < count[RANGED_UNIT] + count[MELEE_UNIT]) {
//                        if (playerView.entityProperties.at(BUILDER_UNIT).initialCost + count[BUILDER_UNIT] <=
//                            resource) {
//                            entityType = BUILDER_UNIT;
//                        }
//                    }
//                } else {
//                    if (entity.entityType == RANGED_BASE &&
//                        playerView.entityProperties.at(RANGED_UNIT).initialCost + count[RANGED_UNIT] <= resource) {
//                        entityType = RANGED_UNIT;
//                    }
//                    if (entity.entityType == MELEE_BASE &&
//                        playerView.entityProperties.at(MELEE_UNIT).initialCost + count[MELEE_UNIT] <= resource) {
//                        entityType = MELEE_UNIT;
//                    }
//                }
//            }
//            if (entityType != HOUSE) {
//                buildAction = std::shared_ptr<BuildAction>(new BuildAction(entityType,
//                                                                           Vec2Int(entity.position.x + properties.size,
//                                                                                   entity.position.y + properties.size -
//                                                                                   1)));
//                resource -= playerView.entityProperties.at(entityType).initialCost + count[entityType];
//            }
//        }
//        std::vector<EntityType> validAutoAttackTargets;
//        if (entity.entityType == BUILDER_UNIT && buildAction == nullptr && repairAction == nullptr) {
//            validAutoAttackTargets.push_back(RESOURCE);
//        }
//        result.entityActions[entity.id] = EntityAction(
//                moveAction,
//                buildAction,
//                std::shared_ptr<AttackAction>(new AttackAction(
//                        nullptr,
//                        std::shared_ptr<AutoAttack>(new AutoAttack(properties.sightRange, validAutoAttackTargets)))),
//                repairAction);


    return result;
}

void MyStrategy::debugUpdate(const PlayerView &playerView, DebugInterface &debugInterface) {
    debugInterface.send(DebugCommand::Clear());
    debugInterface.getState();
}
