//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_MCMF_HPP
#define AICUP_CPP_MCMF_HPP

#include <queue>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>

using namespace std;

/*
* Problems:
* 1. http://codeforces.com/contest/708/problem/D
*/

const int MAX_E = (int) 2e5;
const int OO = (int) 1e5;
class MCMF {
public:
    int n, s, t, E;
    vector<int> adj, nxt, lst, frm, cap, flw, cst, dst, pot;
    int totalCost, totalFlow;
    MCMF();
    void init(int nn, int ss, int tt);

    void add(int u, int v, int ca, int co);

    void bellman();

    int dijkstra();

    int mincost();

    vector <pair<int, int>> getPairs(int n1, int m1);
};

#endif //AICUP_CPP_MCMF_HPP
