#include "graph.hpp"
#include "subgraph_module.hpp"
#include "bfs.hpp"
#include "dijkstra.hpp"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <vector>

void printUsage(const char* prog) {
    std::cout << "Uso: " << prog << " -m <modulo> [-d dataset] [-o output]" << std::endl;
    std::cout << "Módulos:" << std::endl;
    std::cout << "  A - Análisis estructural del grafo" << std::endl;
    std::cout << "  B - Camino mínimo P2P (Dijkstra y BFS)" << std::endl;
    std::cout << "  C - Análisis de subgrafo inducido" << std::endl;
    std::cout << std::endl;
    std::cout << "Opciones:" << std::endl;
    std::cout << "  -d  Dataset (default: ../utils/roadNet.txt)" << std::endl;
    std::cout << "  -o  Directorio de salida (default: ../results)" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }

    std::string module;
    std::string dataset = "../utils/roadNet.txt";
    std::string outputDir = "../results";

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-m" && i + 1 < argc) {
            module = argv[++i];
        } else if (arg == "-d" && i + 1 < argc) {
            dataset = argv[++i];
        } else if (arg == "-o" && i + 1 < argc) {
            outputDir = argv[++i];
        }
    }

    if (module == "A") {
        std::cout << "=== Módulo A: Análisis Estructural ===" << std::endl;
        std::cout << "Cargando dataset: " << dataset << std::endl;

        Graph g;
        if (!g.loadCurated(dataset)) {
            std::cerr << "Error cargando el dataset" << std::endl;
            return 1;
        }

        std::filesystem::create_directories(outputDir);
        std::cout << "Dataset cargado exitosamente" << std::endl;
        std::cout << "Nodos: " << g.getNumNodes() << std::endl;
        std::cout << "Aristas: " << g.getNumEdges() << std::endl;

        std::string outputPath = outputDir + "/analisis_estructural.txt";
        g.writeStructuralAnalysis(outputPath);

    } else if (module == "B") {
        std::cout << "=== Módulo B: Camino Mínimo P2P ===" << std::endl;
        std::cout << "Cargando dataset: " << dataset << std::endl;

        Graph g;
        if (!g.loadCuratedWithWeights(dataset)) {
            std::cerr << "Error cargando el dataset" << std::endl;
            return 1;
        }

        std::filesystem::create_directories(outputDir);

        // consultas fijas segun el enunciado (todos los equipos deben obtener los mismos resultados)
        std::vector<std::tuple<std::string, int, int>> queries = {
            {"Q01", 1,      500000},
            {"Q02", 100,    1000000},
            {"Q03", 50000,  750000},
            {"Q04", 200000, 800000},
            {"Q05", 300000, 100000},
            {"Q06", 1,      1087562},
            {"Q07", 500000, 1},
            {"Q08", 250000, 600000},
            {"Q09", 10000,  900000},  // origen es 10000, no 1000
            {"Q10", 400000, 150000}
        };

        std::string csvPath = outputDir + "/consultas_p2p.csv";
        std::ofstream csv(csvPath);
        if (!csv.is_open()) {
            std::cerr << "No se pudo crear: " << csvPath << std::endl;
            return 1;
        }

        csv << "consulta,origen,destino,dist_dijkstra,saltos_bfs,nodos_dijkstra,nodos_bfs,t_dijkstra_ms,t_bfs_ms\n";
        csv << std::fixed << std::setprecision(2);

        double totalDijkstraMs = 0.0;
        double totalBfsMs = 0.0;

        for (const auto& [consulta, origen, destino] : queries) {
            int srcIdx = g.getInternalIdx(origen);
            int dstIdx = g.getInternalIdx(destino);

            // si alguno de los nodos no existe en el grafo, reportamos INF
            if (srcIdx == -1 || dstIdx == -1) {
                csv << consulta << "," << origen << "," << destino
                    << ",INF,-1,0,0,0.00,0.00\n";
                std::cout << consulta << " saltada (nodo no existe)" << std::endl;
                continue;
            }

            DijkstraResult dj = dijkstra(g.getAdjWList(), srcIdx, dstIdx, g.getNumNodes());
            BFSResult bfs = bfsPath(g.getAdjList(), srcIdx, dstIdx, g.getNumNodes());

            totalDijkstraMs += dj.timeMs;
            totalBfsMs += bfs.timeMs;

            csv << consulta << "," << origen << "," << destino << ",";
            if (dj.distance == INF_DIST) {
                csv << "INF,";
            } else {
                csv << dj.distance << ",";
            }
            csv << bfs.hops << ","
                << dj.nodesExplored << ","
                << bfs.nodesExplored << ","
                << dj.timeMs << ","
                << bfs.timeMs << "\n";

            // guardamos el camino completo para Q01 y Q06 con ids originales
            if (consulta == "Q01" || consulta == "Q06") {
                std::string pathFile = outputDir + "/camino_" + consulta + ".txt";
                std::ofstream pathOut(pathFile);
                if (pathOut.is_open()) {
                    if (dj.path.empty()) {
                        pathOut << "Sin camino\n";
                    } else {
                        for (size_t i = 0; i < dj.path.size(); i++) {
                            pathOut << g.getOriginalId(dj.path[i]);
                            if (i + 1 < dj.path.size()) pathOut << " ";
                        }
                        pathOut << "\n";
                    }
                    pathOut.close();
                }
            }

            std::cout << consulta << " procesada" << std::endl;
        }

        csv.close();
        std::cout << "Resultados en: " << csvPath << std::endl;

        // tiempos.csv: resumen consolidado de tiempos del modulo B
        std::ofstream tiempos(outputDir + "/tiempos.csv");
        if (tiempos.is_open()) {
            tiempos << std::fixed << std::setprecision(2);
            tiempos << "modulo,algoritmo,tiempo_total_ms\n";
            tiempos << "B,dijkstra," << totalDijkstraMs << "\n";
            tiempos << "B,bfs," << totalBfsMs << "\n";
            tiempos.close();
        }

    } else if (module == "C") {
        std::cout << "=== Modulo C: Analisis de Subgrafo ===" << std::endl;
        std::cout << "Cargando dataset: " << dataset << std::endl;

        Graph g;
        if (!g.loadCuratedWithWeights(dataset)) {
            std::cerr << "Error cargando el dataset" << std::endl;
            return 1;
        }

        std::filesystem::create_directories(outputDir);

        // recalculamos dijkstra para Q01 y Q06 con los ids reales del enunciado
        // devuelve indices internos para que SubgraphModule pueda indexar adjW
        std::cout << "Calculando camino Q01 (1 -> 500000)..." << std::endl;
        std::vector<int> pathQ01 = g.dijkstra(1, 500000);

        std::cout << "Calculando camino Q06 (1 -> 1087562)..." << std::endl;
        std::vector<int> pathQ06 = g.dijkstra(1, 1087562);

        if (pathQ01.empty()) {
            std::cerr << "Advertencia: Q01 sin camino (nodos en componentes distintas)" << std::endl;
        }
        if (pathQ06.empty()) {
            std::cerr << "Advertencia: Q06 sin camino (nodos en componentes distintas)" << std::endl;
        }

        SubgraphModule modC;
        modC.runModuleC(g, pathQ01, pathQ06, outputDir);

    } else {
        std::cerr << "Módulo '" << module << "' no reconocido. Usar A, B o C." << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}
