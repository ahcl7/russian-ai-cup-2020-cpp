//
// Created by Apple on 14/11/2020.
//

//https://github.com/chemthan/chemthan.git
#include "mcmf.hpp"

using namespace std;

void MCMF::init(int nn, int ss, int tt) {
    n = nn, s = ss, t = tt;
    fill_n(lst, n, -1), E = 0;
}

void MCMF::add(int u, int v, int ca, int co) {
    adj[E] = v, cap[E] = ca, flw[E] = 0, cst[E] = +co, nxt[E] = lst[u], lst[u] = E++;
    adj[E] = u, cap[E] = 0, flw[E] = 0, cst[E] = -co, nxt[E] = lst[v], lst[v] = E++;
}

int MCMF::spfa() {
    fill_n(dst, n, coo), dst[s] = 0;
    queue<int> que;
    que.push(s);
    while (que.size()) {
        int u = que.front();
        que.pop();
        for (int e = lst[u]; e != -1; e = nxt[e])
            if (flw[e] < cap[e]) {
                int v = adj[e];
                if (dst[v] > dst[u] + cst[e]) {
                    dst[v] = dst[u] + cst[e];
                    frm[v] = e;
                    if (!vis[v]) {
                        vis[v] = 1;
                        que.push(v);
                    }
                }
            }
        vis[u] = 0;
    }
    return dst[t] < coo;
}

int MCMF::mincost() {
    totalCost = 0, totalFlow = 0;
    while (1) {
        if (!spfa()) break;
        int mn = foo;
        for (int v = t, e = frm[v]; v != s; v = adj[e ^ 1], e = frm[v]) mn = min(mn, cap[e] - flw[e]);
        for (int v = t, e = frm[v]; v != s; v = adj[e ^ 1], e = frm[v]) {
            flw[e] += mn;
            flw[e ^ 1] -= mn;
        }
        totalFlow += mn;
        totalCost += mn * dst[t];
    }
    return totalCost;
}

vector <pair<int, int>> MCMF::getPairs(int n, int m) {
    vector <pair<int, int>> res;
    for (int i = 0; i < E; i += 2) {
        int v = adj[i];
        int u = adj[i ^ 1];
        if (u < n && (v >= n && v < n + m)) {
            res.push_back(make_pair(u, v));
        }
    }
    return res;
}