#include "dijkstra.hpp"
#include "graph.hpp"
#include <queue>
#include <chrono>
#include <algorithm>

DijkstraResult dijkstra(
    const std::vector<std::vector<std::pair<int,int>>>& adjW,
    int srcIdx,
    int dstIdx,
    int numNodes
)
{
    DijkstraResult result;
    result.distance = INF_DIST;
    result.nodesExplored = 0;

    //inicializacion

    std::vector<long long> dist(numNodes, INF_DIST);
    std::vector<int> prev(numNodes, -1);

    //std::greater<PairLI> -> usamos una cola de prioridad para que el par con menor primera componente quede en el tope

    using PairLI = std::pair<long long, int>;
    std::priority_queue<PairLI, std::vector<PairLI>, std::greater<PairLI>> pq;

    dist[srcIdx] = 0;
    pq.push({0LL, srcIdx});

    //reloj de inicio
    auto t0 = std::chrono::high_resolution_clock::now();

    //algoritmo principal de dijkstra

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > dist[u]) continue;
        result.nodesExplored++;

        if (u == dstIdx) break;
        
        for (auto& [v, w] : adjW[u]) {
            long long newDist = dist[u] + static_cast<long long>(w);
            if (newDist < dist[v]) {
                dist[v] = newDist;
                prev[v] = u;
                pq.push({newDist, v});
            }
        } 
    }

    //reloj de fin
    auto t1 = std::chrono::high_resolution_clock::now();
    result.timeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
    result.distance = dist[dstIdx]; // si no llega, entonces -> INF_DIST

    //si el destino es alcanzable, reconstruimos
    if (dist[dstIdx] != INF_DIST) {
        std::vector<int> path;
        for (int cur = dstIdx; cur != -1; cur = prev[cur]) {
            path.push_back(cur);
            if (cur == srcIdx) break;
        }
        std::reverse(path.begin(), path.end());
        result.path = std::move(path);
    }
    return result;
}