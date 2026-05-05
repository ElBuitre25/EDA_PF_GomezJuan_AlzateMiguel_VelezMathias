#include "graph.hpp"
#include <unordered_set>  // para usar el set que no permite repetidos
#include <cstdlib> // srand y rand para los pesos aleatorios

// esta funcion lee el archivo una primera vez solo para mapear los ids
// porque los nodos no estan numerados del 0 al final, hay huecos
void Graph::reindex(const std::string& filename) {
    std::unordered_set<int> uniqueIds;  // aqui guardamos los ids unicos (sin repetir)
    std::ifstream file(filename);
    if (!file.is_open()) return;  // si no se puede abrir, salimos

    std::string line;
    std::getline(file, line); // saltamos la primera linea que dice "nodos aristas"

    // leemos todo el archivo para encontrar todos los ids unicos
    while (std::getline(file, line)) {
        if (line.empty()) continue;  // saltamos lineas vacias
        std::stringstream ss(line);
        int u, v;
        if (ss >> u >> v) {
            uniqueIds.insert(u);  // guardamos el nodo u
            uniqueIds.insert(v);  // guardamos el nodo v
        }
    }
    file.close();

    // pasamos los ids unicos a un vector para ordenarlos
    std::vector<int> sortedIds(uniqueIds.begin(), uniqueIds.end());
    std::sort(sortedIds.begin(), sortedIds.end());  // los ordenamos de menor a mayor
    numNodes = sortedIds.size();  // aqui ya sabemos cuantos nodos unicos hay
    idxToId = sortedIds;  // guardamos: posicion 0 -> id original, posicion 1 -> id original, etc.
    idToIdx.clear();
    // creamos el mapeo inverso: id original -> posicion en el vector (0, 1, 2...)
    for (int i = 0; i < numNodes; i++) {
        idToIdx[sortedIds[i]] = i;
    }
}

// esta funcion carga el grafo desde el archivo curado
bool Graph::loadCurated(const std::string& filename) {
    reindex(filename);  // primero hacemos el mapeo de ids
    adj.assign(numNodes, std::vector<int>());  // creamos la lista de adyacencia vacia

    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    std::getline(file, line); // saltamos la primera linea

    numEdges = 0;
    // leemos cada arista y la convertimos a indices internos
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        int u, v;
        if (ss >> u >> v) {
            int idx_u = idToIdx[u];  // convertimos id original a indice 0-based
            int idx_v = idToIdx[v];  // lo mismo para v
            adj[idx_u].push_back(idx_v);  // agregamos v a la lista de u
            adj[idx_v].push_back(idx_u);  // como es no dirigido, tambien u a v
            numEdges++;
        }
    }
    file.close();
    return true;
}

// loadCuratedWithWeights: cargar el grafo con pesos aleatorios
// para cada arista, se genera un solo peso = (rand() % 10) + 1

bool Graph::loadCuratedWithWeights(const std::string& filename) {
    reindex(filename); 

    adj.assign(numNodes, std::vector<int>());
    adjW.assign(numNodes, std::vector<std::pair<int,int>>()); 

    std::ifstream file(filename);
    if (!file.is_open()) return false;

    //srand(42) antes de parsear
    srand(42);
    std::string line;
    std::getline(file, line);

    numEdges = 0;
    while(std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        int u, v;
        if (ss >> u >> v) {
            int iu = idToIdx[u];
            int iv = idToIdx[v];
            int w = (rand() % 10) + 1; // peso aleatorio entre 1 y 10

            // lista no ponderada (BFS)
            adj[iu].push_back(iv);
            adj[iv].push_back(iu);

            //lista ponderada (Dijkstra)
            adjW[iu].emplace_back(iv, w);
            adjW[iv].emplace_back(iu, w);

            numEdges++;

        }
    }
    file.close();
    return true;
}

// encuentra el nodo que tiene mas vecinos (mayor grado)
int Graph::getMaxDegreeNode() const {
    int maxNode = 0;
    size_t maxDegree = 0;
    for (int i = 0; i < numNodes; i++) {
        if (adj[i].size() > maxDegree) {
            maxDegree = adj[i].size();  // actualizamos el grado maximo
            maxNode = i;  // guardamos que nodo tiene ese grado
        }
    }
    return maxNode;
}

// calcula el promedio de grados: (2 * aristas) / nodos
// se multiplica por 2 porque cada arista cuenta para dos nodos
double Graph::getAverageDegree() const {
    if (numNodes == 0) return 0.0;
    return (2.0 * numEdges) / numNodes;
}

// cuenta cuantas componentes conexas hay usando bfs
int Graph::countConnectedComponents() {
    std::vector<bool> visited(numNodes, false);  // marcamos que nodos hemos visitado
    int components = 0;
    largestComponentSize = 0;

    // recorremos todos los nodos
    for (int i = 0; i < numNodes; i++) {
        if (!visited[i]) {  // si no hemos visitado este nodo, es una nueva componente
            components++;
            int componentSize = 0;
            std::queue<int> q;  // usamos una cola para bfs
            q.push(i);
            visited[i] = true;

            // bfs estandar
            while (!q.empty()) {
                int u = q.front();
                q.pop();
                componentSize++;  // contamos cuantos nodos tiene esta componente
                for (int v : adj[u]) {
                    if (!visited[v]) {
                        visited[v] = true;
                        q.push(v);
                    }
                }
            }
            // actualizamos el tamaño de la componente mas grande
            if (componentSize > largestComponentSize) {
                largestComponentSize = componentSize;
            }
        }
    }
    return components;
}

// bfs para encontrar la distancia maxima desde un nodo inicial
// sirve para estimar el diametro del grafo
int Graph::bfsMaxDistance(int startIdx) {
    if (startIdx < 0 || startIdx >= numNodes) return 0;

    std::vector<int> dist(numNodes, -1);  // -1 significa que no hemos llegado a ese nodo
    std::queue<int> q;

    dist[startIdx] = 0;  // la distancia a si mismo es 0
    q.push(startIdx);

    int maxDist = 0;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        if (dist[u] > maxDist) maxDist = dist[u];  // actualizamos la distancia maxima
        for (int v : adj[u]) {
            if (dist[v] == -1) {  // si no hemos visitado v
                dist[v] = dist[u] + 1;  // su distancia es la de u + 1
                q.push(v);
            }
        }
    }
    return maxDist;
}

// escribe el analisis estructural en un archivo de texto
void Graph::writeStructuralAnalysis(const std::string& outputPath) {
    int maxDegIdx = getMaxDegreeNode();  // nodo con mayor grado (indice interno)
    int maxDegOriginalId = getOriginalId(maxDegIdx);  // lo convertimos a id original
    double avgDeg = getAverageDegree();
    int components = countConnectedComponents();
    int mainComponentSize = getLargestComponentSize();
    int diameter = bfsMaxDistance(maxDegIdx);  // estimacion del diametro

    std::ofstream out(outputPath);
    if (!out.is_open()) {
        std::cerr << "error: no se pudo escribir " << outputPath << std::endl;
        return;
    }

    // escribimos las estadisticas comparandolas con snap
    out << "=== analisis estructural del grafo ===" << std::endl;
    out << std::endl;
    out << "estadistica                        valor snap        valor obtenido" << std::endl;
    out << "------------------------------------------------------------" << std::endl;
    out << "numero de nodos                    1,088,092          " << numNodes << std::endl;
    out << "numero de aristas (no dirigidas)   1,541,898          " << numEdges << std::endl;
    out << "promedio de grado                   ~2.83              " << avgDeg << std::endl;
    out << "nodo de mayor grado                (identificar)       " << maxDegOriginalId << " (indice: " << maxDegIdx << ", grado: " << adj[maxDegIdx].size() << ")" << std::endl;
    out << std::endl;
    out << "componentes conexas: " << components << std::endl;
    out << "nodos en componente principal: " << mainComponentSize << " (snap: 1,087,562)" << std::endl;
    out << "diametro aproximado (bfs desde nodo de mayor grado): " << diameter << " (snap: 782)" << std::endl;
    out << std::endl;
    out << "explicacion del diametro:" << std::endl;
    out << "- snap reporta 782 como el diametro (camino mas largo entre todos los pares)." << std::endl;
    out << "- nuestra estimacion usa bfs desde un solo nodo (el de mayor grado)." << std::endl;
    out << "- para obtener el diametro real se requiere bfs desde cada nodo." << std::endl;

    out.close();
    std::cout << "analisis guardado en: " << outputPath << std::endl;
}
