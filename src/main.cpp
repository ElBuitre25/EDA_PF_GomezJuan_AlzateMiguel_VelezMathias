#include "graph.hpp"
#include <iostream>
#include <cstdlib>

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

    } else {
        std::cout << "Módulo " << module << " no implementado aún" << std::endl;
        return 0;
    }

    return 0;
}
