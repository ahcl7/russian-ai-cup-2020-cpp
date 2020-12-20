#ifndef _MY_STRATEGY_HPP_
#define _MY_STRATEGY_HPP_

#include "DebugInterface.hpp"
#include "model/Model.hpp"
#include "Constances.hpp"
#include "Utils.hpp"
#include "manager/Manager.hpp"
#include <exception>
#include <iostream>
#include <map>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <set>
#include <bitset>
#include <unordered_map>


class MyStrategy {
public:
    MyStrategy();
    Action getAction(const PlayerView& playerView, DebugInterface* debugInterface);
    void debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface);
};

#endif