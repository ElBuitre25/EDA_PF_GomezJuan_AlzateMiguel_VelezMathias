#ifndef SUBGRAPH_MODULE_HPP
#define SUBGRAPH_MODULE_HPP

#include "graph.hpp"
#include <vector>
#include <string>

class SubgraphModule {
public:
    // recibe los caminos como indices internos (no ids originales)
    void runModuleC(const Graph& g,
                    const std::vector<int>& path1,
                    const std::vector<int>& path2,
                    const std::string& outputDir);
};

#endif