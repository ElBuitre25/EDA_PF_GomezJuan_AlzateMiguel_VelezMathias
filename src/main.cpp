#include "graph.hpp"
#include <iostream>
#include <cstdlib>

#include "bfs.hpp"
#include "dijkstra.hpp"
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
        std::vector<std::tuple<std::string, int, int>> queries = {
            {"Q01", 1, 500000},
            {"Q02", 100, 1000000},
            {"Q03", 50000, 750000},
            {"Q04", 200000, 800000},
            {"Q05", 300000, 100000},
            {"Q06", 1, 1087562},
            {"Q07", 500000, 1},
            {"Q08", 250000, 600000},
            {"Q09", 1000, 900000},
            {"Q10", 400000, 150000}
        };

        std::string csvPath = outputDir + "/consultas_p2p.csv";
        std::ofstream csv(csvPath);

        if (!csv.is_open()) {
            std::cerr << "No se pudo crear el archivo: " << csvPath << std::endl;
            return 1;
        }

        csv << "consulta,origen,destino,dist_dijkstra,saltos_bfs,nodos_dijkstra,nodos_bfs,t_dijkstra_ms,t_bfs_ms\n";
        csv << std::fixed << std::setprecision(2);

        for(const auto& [consulta, origen, destino] : queries) {
            int srcIdx = g.getInternalIdx(origen);
            int dstIdx = g.getInternalIdx(destino);

            if (srcIdx == -1 || dstIdx == -1) {
                csv << consulta << ","
                    << origen << ","
                    << destino << ","
                    << "INF" << ","
                    << -1 << ","
                    << 0 << ","
                    << 0 << ","
                    << 0.00 << ","
                    << 0.00 << "\n";
                continue;
        }

        DijkstraResult dj = dijkstra(g.getAdjWList(), srcIdx, dstIdx, g.getNumNodes());
        BFSResult bfs = bfsPath(g.getAdjList(), srcIdx, dstIdx, g.getNumNodes());

        csv << consulta << ","
            << origen << ","
            << destino << ",";
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
        if (consulta == "Q01" || consulta == "Q06"){
            std::string pathFile = outputDir + "/camino_" + consulta + ".txt";
            std::ofstream pathOut(pathFile);

            if (pathOut.is_open()) {
                if (dj.path.empty()) {
                    pathOut << "Sin camino\n";
                } else {
                    for (size_t i = 0; i < dj.path.size(); i++) {
                        pathOut << g.getOriginalId(dj.path[i]);

                        if (i + 1 < dj.path.size()) {
                            pathOut << " ";
                        }
                    }
                    pathOut << "\n";
                }
            }
        }

        std::cout << consulta << " procesada" << std::endl;
    }

    csv.close();
    std::cout << "Resultados guardados en: " << csvPath << std::endl;

    } else {
        std::cout << "Módulo " << module << " no implementado aún" << std::endl;
        return 0;
    }

    return 0;
}
