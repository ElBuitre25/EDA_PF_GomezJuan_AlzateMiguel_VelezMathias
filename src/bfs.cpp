#include "bfs.hpp"

#include <queue>
#include <vector>
#include <chrono>


BFSResult bfsPath(
    const std::vector<std::vector<int>>& adj,
    int srcIdx,
    int dstIdx,
    int numNodes
)
{
    BFSResult result;
    result.hops = -1;
    result.nodesExplored = 0;

    std::vector<int> dist(numNodes, -1);
    std::queue<int> q;

    dist[srcIdx] = 0;
    q.push(srcIdx);

    // inicio reloj

    auto t0 = std::chrono::high_resolution_clock::now();

    // bucle while
    while (!q.empty()) {
        int u = q.front();
        q.pop();

        result.nodesExplored++;
        if (u == dstIdx) {
            result.hops = dist[dstIdx];
            break;
        } 

        for(int nb : adj[u]) {
            if (dist[nb] == -1) {
                dist[nb] = dist[u] + 1;
                q.push(nb);
            }
        }
    }
    // fin del reloj
    auto t1 = std::chrono::high_resolution_clock::now();
    result.timeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();

    return result;
}