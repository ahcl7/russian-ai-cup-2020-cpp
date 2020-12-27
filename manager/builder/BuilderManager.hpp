//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_BUILDERMANAGER_HPP
#define AICUP_CPP_BUILDERMANAGER_HPP

#include "../../model/Model.hpp"
#include "../../Constances.hpp"
#include "InformationForBuilderManager.hpp"
#include "../bitmap/BitMap.hpp"
#include "../../Utils.hpp"
#include "../../mcmf.hpp"
#include <map>
#include <bitset>
#include <vector>
using namespace std;


class BuilderManager {
private:
    void assignTasks(vector<BuilderTask>& tasks);
public:

    InformationForBuilderManager infoFBM;
    MCMF mcmf;
    BuilderManager() {};
    BuilderManager(const PlayerView& playerView);
    EntityAction getAction(int entityId);
    void createAndAssignTasks();
    void update(const PlayerView& playerView);
    void implement(vector<Entity>& buildersCanBeInvolvedForTasks, vector<BuilderTask>& tasks, vector<pair<int,int>>& assignment);
    Vec2Int getClosetResource(Vec2Int position, vector <Entity> resourceEntities);
};

#endif //AICUP_CPP_BUILDERMANAGER_HPP
