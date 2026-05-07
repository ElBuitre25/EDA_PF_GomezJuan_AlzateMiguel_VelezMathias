#include "graph.hpp"
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include "subgraph_module.hpp"

// manejo de conjuntos para evitar ciclos en el MST
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

// estructura para las aristas del recorte con pesos
struct SubEdge {
    int u, v, weight;
    bool operator<(const SubEdge& other) const {
        return weight < other.weight;
    }
};

class SubgraphModule {
private:
    // revisa si hay ciclos con DFS y colores para ver si es DAG
    bool hasCycleDFS(int u, std::map<int, int>& color, const std::map<int, std::vector<int>>& adjSub) {
        color[u] = 1; // marcado como visitando
        if (adjSub.count(u)) {
            for (int v : adjSub.at(u)) {
                if (color[v] == 1) return true; // ciclo encontrado
                if (color[v] == 0 && hasCycleDFS(v, color, adjSub)) return true;
            }
        }
        color[u] = 2; // terminado
        return false;
    }

public:
    void runModuleC(const Graph& g, const std::vector<int>& pathQ01, const std::vector<int>& pathQ06) {
        // metemos nodos de las rutas en un set para que no se repitan
        std::set<int> uniqueNodes;
        for (int node : pathQ01) uniqueNodes.insert(node);
        for (int node : pathQ06) uniqueNodes.insert(node);

        std::cout << "Nodos en el subgrafo: " << uniqueNodes.size() << std::endl;

        std::vector<SubEdge> edges;
        const auto& fullAdjW = g.getAdjWList();
        
        // buscamos conexiones originales solo entre nuestros nodos
        for (int u : uniqueNodes) {
            for (auto& edge : fullAdjW[u]) {
                int v = edge.first;
                int w = edge.second;
                if (uniqueNodes.count(v) && u < v) {
                    edges.push_back({u, v, w});
                }
            }
        }

        // kruskal: ordenamos aristas y unimos con DSU
        std::sort(edges.begin(), edges.end());
        DSU dsu(g.getNumNodes());
        int mstWeight = 0;
        int edgesInMST = 0;

        for (auto& e : edges) {
            if (dsu.find(e.u) != dsu.find(e.v)) {
                dsu.unite(e.u, e.v);
                mstWeight += e.weight;
                edgesInMST++;
            }
        }

        // armamos adj de nuevo para chequear ciclos
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

        // prints de resultados
        std::cout << "=== Resultados Modulo C ===" << std::endl;
        std::cout << "Peso total del MST: " << mstWeight << std::endl;
        std::cout << "Aristas en el MST: " << edgesInMST << std::endl;
        std::cout << "Es un DAG: " << (isDAG ? "Si" : "No") << std::endl;
    }
};