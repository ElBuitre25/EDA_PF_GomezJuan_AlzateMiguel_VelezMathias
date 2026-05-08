#include "subgraph_module.hpp"
#include "graph.hpp"
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>

// DSU con union por rango para kruskal, evita ciclos
struct DSU {
    std::vector<int> parent, rank_;
    DSU(int n) : parent(n), rank_(n, 0) {
        for (int i = 0; i < n; i++) parent[i] = i;
    }
    int find(int i) {
        if (parent[i] != i) parent[i] = find(parent[i]);
        return parent[i];
    }
    bool unite(int i, int j) {
        int a = find(i), b = find(j);
        if (a == b) return false; // mismo componente, saltamos
        if (rank_[a] < rank_[b]) std::swap(a, b);
        parent[b] = a;
        if (rank_[a] == rank_[b]) rank_[a]++;
        return true;
    }
};

// arista del subgrafo con peso para ordenar en kruskal
struct SubEdge {
    int u, v, weight;
    bool operator<(const SubEdge& o) const { return weight < o.weight; }
};

// DFS con colores: 0=no visitado, 1=en proceso, 2=terminado
// retorna true si hay back edge (ciclo)
static bool hasCycleDFS(int u, std::map<int, int>& color,
                         const std::map<int, std::vector<int>>& adjSub) {
    color[u] = 1;
    if (adjSub.count(u)) {
        for (int v : adjSub.at(u)) {
            if (color[v] == 1) return true;
            if (color[v] == 0 && hasCycleDFS(v, color, adjSub)) return true;
        }
    }
    color[u] = 2;
    return false;
}

void SubgraphModule::runModuleC(const Graph& g,
                                 const std::vector<int>& pathQ01,
                                 const std::vector<int>& pathQ06,
                                 const std::string& outputDir) {
    // juntamos nodos de ambos caminos en un set para no repetir
    // los paths ya vienen en indices internos
    std::set<int> uniqueNodes;
    for (int node : pathQ01) uniqueNodes.insert(node);
    for (int node : pathQ06) uniqueNodes.insert(node);

    // buscamos aristas del grafo original que conecten nodos del subgrafo
    // u < v para no duplicar aristas en grafo no dirigido
    std::vector<SubEdge> edges;
    const auto& fullAdjW = g.getAdjWList();
    for (int u : uniqueNodes) {
        for (auto& [v, w] : fullAdjW[u]) {
            if (uniqueNodes.count(v) && u < v) {
                edges.push_back({u, v, w});
            }
        }
    }

    // kruskal: ordenar por peso y unir con DSU
    std::sort(edges.begin(), edges.end());
    DSU dsu(g.getNumNodes());
    int mstWeight = 0;
    int edgesInMST = 0;
    for (auto& e : edges) {
        if (dsu.unite(e.u, e.v)) {
            mstWeight += e.weight;
            edgesInMST++;
        }
    }

    // para el DAG check usamos la adj dirigida (solo u->v, no v->u)
    // si hubieramos puesto ambas direcciones siempre habria ciclo
    std::map<int, std::vector<int>> adjSub;
    for (auto& e : edges) {
        adjSub[e.u].push_back(e.v);
    }

    bool isDAG = true;
    std::map<int, int> color;
    for (int node : uniqueNodes) {
        if (color[node] == 0) {
            if (hasCycleDFS(node, color, adjSub)) {
                isDAG = false;
                break;
            }
        }
    }

    // subgrafo_caminos.txt: formato compatible con Evaluacion 03
    // primera linea: num_nodos num_aristas
    // resto: id_original_u id_original_v peso
    std::ofstream subOut(outputDir + "/subgrafo_caminos.txt");
    if (subOut.is_open()) {
        subOut << uniqueNodes.size() << " " << edges.size() << "\n";
        for (auto& e : edges) {
            subOut << g.getOriginalId(e.u) << " "
                   << g.getOriginalId(e.v) << " "
                   << e.weight << "\n";
        }
        subOut.close();
    }

    // analisis_subgrafo.txt: estadisticas del subgrafo
    std::ofstream anaOut(outputDir + "/analisis_subgrafo.txt");
    if (anaOut.is_open()) {
        anaOut << "=== analisis del subgrafo inducido (Q01 + Q06) ===" << "\n\n";
        anaOut << "nodos en el subgrafo: " << uniqueNodes.size() << "\n";
        anaOut << "aristas en el subgrafo: " << edges.size() << "\n";
        anaOut << "aristas en el MST (Kruskal): " << edgesInMST << "\n";
        anaOut << "peso total del MST: " << mstWeight << "\n";
        anaOut << "es un DAG (representacion dirigida u->v): "
               << (isDAG ? "si" : "no") << "\n\n";
        anaOut << "nota: el subgrafo original es no dirigido, por lo que siempre\n";
        anaOut << "tiene ciclos. el check DAG se hace sobre aristas dirigidas (u < v)\n";
        anaOut << "para verificar la estructura de los caminos minimos.\n";
        anaOut.close();
    }

    std::cout << "=== Resultados Modulo C ===" << std::endl;
    std::cout << "Nodos en el subgrafo: " << uniqueNodes.size() << std::endl;
    std::cout << "Aristas en el subgrafo: " << edges.size() << std::endl;
    std::cout << "Peso total del MST: " << mstWeight << std::endl;
    std::cout << "Aristas en el MST: " << edgesInMST << std::endl;
    std::cout << "Es un DAG: " << (isDAG ? "Si" : "No") << std::endl;
    std::cout << "Resultados en: " << outputDir << std::endl;
}
