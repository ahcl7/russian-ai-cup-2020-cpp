//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP2020_INFORMATIONFORBASEMANAGER_HPP
#define AICUP2020_INFORMATIONFORBASEMANAGER_HPP

#endif //AICUP2020_INFORMATIONFORBASEMANAGER_HPP

#include "../../model/Model.hpp"
#include "../../Utils.hpp"
#include <vector>
using namespace std;

class InformationForBaseManager {
public:
    int numberOfBuilders;
    int numberOfAttackers;
    InformationForBaseManager() {};
    InformationForBaseManager(const PlayerView& playerView);
    void update(const PlayerView& playerView);
};