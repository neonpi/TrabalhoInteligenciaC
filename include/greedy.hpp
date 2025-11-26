#ifndef __GREEDY_HPP__
#define __GREEDY_HPP__

#include "dependency_graph.hpp"

namespace Greedy {
    std::vector<int> greedyRandomizedAdaptiveProcedure(const DependencyGraph& dependency_graph, float alpha, unsigned int seed);
    int calculateTimespan(const DependencyGraph& dependency_graph, const std::vector<int>& schedule);
}

#endif
