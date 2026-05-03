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

class Graph {
private:
    std::vector<std::vector<int>> adj;  // lista de adyacencia: adj[i] = vecinos del nodo i
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
    int getNumNodes() const { return numNodes; }
    int getNumEdges() const { return numEdges; }
    const std::vector<std::vector<int>>& getAdjList() const { return adj; }

    int getMaxDegreeNode() const;  // encuentra el nodo con mas vecinos
    double getAverageDegree() const;  // calcula el grado promedio
    int countConnectedComponents();  // cuenta las componentes conexas
    int getLargestComponentSize() const { return largestComponentSize; }
    int bfsMaxDistance(int startNode);  // bfs para estimar diametro
    void writeStructuralAnalysis(const std::string& outputPath);  // escribe resultados
    int getOriginalId(int idx) const { return idxToId[idx]; }  // convierte indice a id original
};

#endif
