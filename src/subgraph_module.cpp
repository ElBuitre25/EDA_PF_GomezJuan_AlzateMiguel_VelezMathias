#include "graph.hpp"
#include <set>
#include <map>

// --- ESTRUCTURA DSU (Union-Find) ---
// Sirve para el Algoritmo de Kruskal (MST)
struct DSU {
    std::vector<int> parent;
    DSU(int n) {
        parent.resize(n);
        for (int i = 0; i < n; i++) parent[i] = i;
    }
    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) parent[root_i] = root_j;
    }
};

// ESTRUCTURA PARA LAS ARISTAS DEL SUBGRAFO
struct SubEdge {
    int u, v, weight;
    bool operator<(const SubEdge& other) const {
        return weight < other.weight;
    }
};

class SubgraphModule {
public:
    // Función principal que deberás llamar desde main.cpp
    void runModuleC(const Graph& g, const std::vector<int>& pathQ01, const std::vector<int>& pathQ06) {
        std::cout << "Iniciando Modulo C: Analisis de Subgrafo..." << std::endl;

        // 1. Crear el conjunto de nodos unicos (Inducido)
        std::set<int> uniqueNodes;
        for (int node : pathQ01) uniqueNodes.insert(node);
        for (int node : pathQ06) uniqueNodes.insert(node);

        std::cout << "Nodos en el subgrafo: " << uniqueNodes.size() << std::endl;

        // 2. Construir el MST usando Kruskal
        // (Aqui vendra la logica de Kruskal que haremos juntos)
        
        // 3. Verificar si es un DAG usando DFS
        // (Aqui vendra la logica de deteccion de ciclos)
    }
};