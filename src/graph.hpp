#ifndef GRAPH_HPP
#define GRAPH_HPP
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <unordered_map>  // para el mapeo de ids
#include <limits>

// valor inicial temporal para el algoritmo de dijkstra, cuando no se conoce la distancia desde el nodo origen hacia algún otro nodo.
static const long long INF_DIST = std::numeric_limits<long long>::max();

class Graph {
private:
    std::vector<std::vector<int>> adj;  // lista de adyacencia: adj[i] = vecinos del nodo i
    std::vector<std::vector<std::pair<int,int>>> adjW; // Lista de adyacencia con pesos: adjW[i] = {(vecino, peso),...}
    int numNodes;  // cantidad de nodos
    int numEdges;  // cantidad de aristas (no dirigidas)
    int largestComponentSize;  // tamaño de la componente conexa mas grande
    std::unordered_map<int, int> idToIdx;  // mapeo: id original -> indice interno (0-based)
    std::vector<int> idxToId;  // mapeo inverso: indice interno -> id original

    void reindex(const std::string& filename);  // funcion para crear el mapeo de ids
    int getIndex(int originalId);  // convierte id original a indice

public:
    Graph() : numNodes(0), numEdges(0), largestComponentSize(0) {}

    bool loadCurated(const std::string& filename);  // carga el grafo desde archivo curado
    bool loadCuratedWithWeights(const std::string& filename); // carga el grafo con pesos aleatorios
    int getNumNodes() const { return numNodes; }
    int getNumEdges() const { return numEdges; }
    const std::vector<std::vector<int>>& getAdjList() const { return adj; }
    const std::vector<std::vector<std::pair<int, int>>>& getAdjWList() const { return adjW;}

    int getMaxDegreeNode() const;  // encuentra el nodo con mas vecinos
    double getAverageDegree() const;  // calcula el grado promedio
    int countConnectedComponents();  // cuenta las componentes conexas
    int getLargestComponentSize() const { return largestComponentSize; }
    int bfsMaxDistance(int startNode);  // bfs para estimar diametro
    void writeStructuralAnalysis(const std::string& outputPath);  // escribe resultados
    int getOriginalId(int idx) const { return idxToId[idx]; }  // convierte indice a id original

    // devuelve -1 si el id original no existe en el grafo
    int getInternalIdx (int originalId) const {
        auto it = idToIdx.find(originalId);
        return (it  != idToIdx.end()) ? it -> second : -1; 
    }
    bool hasOriginalId(int originalId) const {
        return idToIdx.count(originalId) > 0;
    }  

    // --- Para que el Modulo C pueda pedirle rutas al Modulo B ---
    std::vector<int> dijkstra(int startId, int endId);
};

#endif
