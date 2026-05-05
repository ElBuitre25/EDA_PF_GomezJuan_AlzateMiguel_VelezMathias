#ifndef BFS_HPP
#define BFS_HPP

#include <vector>
#include <string>

struct BFSResult {
    int hops; // numero de saltos al destino (aristas)
    int nodesExplored; // nodos visitados en la busqueda
    double timeMs; // tiempo en milisegundos
};

// adj: lista adyacencia no ponderada
// srcIdx: indice nodo origen
// dstIdx: indice nodo destino
// numNodes: numero total de nodos

BFSResult bfsPath(
    const std::vector<std::vector<int>>& adj,
    int srcIdx,
    int dstIdx,
    int numNodes
);
#endif