#ifndef SUBGRAPH_MODULE_HPP
#define SUBGRAPH_MODULE_HPP

#include "graph.hpp"
#include <vector>

class SubgraphModule {
public:
    void runModuleC(Graph& g, std::vector<int>& path1, std::vector<int>& path2);
};

#endif