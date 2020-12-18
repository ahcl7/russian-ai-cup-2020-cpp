//
// Created by Apple on 14/11/2020.
//

#ifndef AICUP_CPP_MCMF_HPP
#define AICUP_CPP_MCMF_HPP

#include <queue>
#include <algorithm>

using namespace std;

/*
* Problems:
* 1. http://codeforces.com/contest/708/problem/D
*/

#define MCMF MincostMaxflow
const int foo = (int) 1e9;
const int coo = (int) 1e9;
namespace MincostMaxflow {
    const int maxv = 1e5 + 5;
    const int maxe = 1e6 + 5;
    int n, s, t, E;
    int adj[maxe], nxt[maxe], lst[maxv], frm[maxv], vis[maxv];
    int cap[maxe], flw[maxe], totalFlow;
    int cst[maxe], dst[maxv], totalCost;

    void init(int nn, int ss, int tt);

    void add(int u, int v, int ca, int co);

    int spfa();

    int mincost();

    vector <pair<int, int>> getPairs(int n, int m);
}

#endif //AICUP_CPP_MCMF_HPP
