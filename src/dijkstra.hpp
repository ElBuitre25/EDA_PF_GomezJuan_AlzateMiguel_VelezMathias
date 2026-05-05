#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include<vector>
#include<string>
#include<limits>

struct DijkstraResult {
    long long distance; // distancia minima ponderada
    int nodesExplored; // nodos cerrados extraidos del heap
    double timeMs; // tiempo en ejecucion
    std::vector<int> path;
    
};

// adjW: lista adyacencia ponderada
// srcIdx: indice del nodo origen
//dstIdx: indice del nodo destino
//numnodes: numero total de nodos

DijkstraResult dijkstra(
    const std::vector<std::vector<std::pair<int,int>>>& adjW,
    int srcIdx,
    int dstIdx,
    int numNodes
);



#endif