//
// Created by Apple on 14/11/2020.
//

//https://github.com/chemthan/chemthan.git
#include "mcmf.hpp"

using namespace std;


MCMF::MCMF() {
    adj = vector<int>(MAX_E, 0);
    cap = vector<int>(MAX_E, 0);
    flw = vector<int>(MAX_E, 0);
    cst = vector<int>(MAX_E, 0);
    nxt = vector<int>(MAX_E, 0);
    lst = vector<int>(MAX_E, 0);
    frm = vector<int>(MAX_E, 0);
    pot = vector<int>(MAX_E, 0);
    dst = vector<int>(MAX_E, 0);
}

void MCMF::init(int nn, int ss, int tt) {
    n = nn, s = ss, t = tt;
    E = 0;
    fill(lst.begin(), lst.begin() + n, -1);
    totalCost = 0;
    totalFlow = 0;
}

void MCMF::add(int u, int v, int ca, int co) {
    adj[E] = v, cap[E] = ca, flw[E] = 0, cst[E] = +co, nxt[E] = lst[u], lst[u] = E++;
    adj[E] = u, cap[E] = 0, flw[E] = 0, cst[E] = -co, nxt[E] = lst[v], lst[v] = E++;
}

void MCMF::bellman() {
    fill(pot.begin(), pot.begin() + n, 0);
    while (1) {
        int found = 0;
        for (int u = 0; u < n; u++)
            for (int e = lst[u]; e != -1; e = nxt[e])
                if (flw[e] < cap[e]) {
                    int v = adj[e];
                    if (pot[v] > pot[u] + cst[e]) {
                        pot[v] = pot[u] + cst[e];
                        found = 1;
                    }
                }
        if (!found) break;
    }
}

int MCMF::dijkstra() {
    priority_queue<pair<int, int> > que;
    fill(dst.begin(), dst.begin() + n, OO), dst[s] = 0;
    que.push(make_pair(-dst[s], s));
    while (que.size()) {
        int dnow = -que.top().first;
        int u = que.top().second;
        que.pop();
        if (dst[u] < dnow) continue;
        for (int e = lst[u]; e != -1; e = nxt[e])
            if (flw[e] < cap[e]) {
                int v = adj[e];
                int dnxt = dnow + cst[e] + pot[u] - pot[v];
                if (dst[v] > dnxt) {
                    dst[v] = dnxt;
                    frm[v] = e;
                    que.push(make_pair(-dnxt, v));
                }
            }
    }
    return dst[t] < OO;
}

int MCMF::mincost() {
    totalCost = 0, totalFlow = 0;
    bellman();
    while (1) {
        if (!dijkstra()) break;
        int mn = OO;
        for (int v = t, e = frm[v]; v != s; v = adj[e ^ 1], e = frm[v]) mn = min(mn, cap[e] - flw[e]);
        for (int v = t, e = frm[v]; v != s; v = adj[e ^ 1], e = frm[v]) {
            flw[e] += mn;
            flw[e ^ 1] -= mn;
        }
        totalFlow += mn;
        totalCost += mn * (dst[t] - pot[s] + pot[t]);
        for (int u = 0; u < n; u++) pot[u] += dst[u];
    }
    return totalCost;
}

vector<pair<int, int>> MCMF::getPairs(int n1, int m1) {
    mincost();
    vector<pair<int, int>> res;
    for (int i = 0; i < E; i += 2) {
        int v = adj[i];
        int u = adj[i ^ 1];
        if (flw[i] && u < n1 && (v >= n1 && v < n1 + m1)) {
            res.push_back(make_pair(u, v - n1));
        }
    }

//    cerr <<"Pairing size:" << res.size() << endl;
    return res;
}