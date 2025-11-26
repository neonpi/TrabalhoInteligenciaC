#include <filesystem>
#include <iostream>

#include "dependency_graph.hpp"
#include "greedy.hpp"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Too many parameters were supplied.\n";

        return 1;
    }

    std::filesystem::path instance_file_path{argv[1]};
    DependencyGraph dependency_graph{instance_file_path};

    auto result = Greedy::greedyRandomizedAdaptiveProcedure(dependency_graph, 0.3, 0);
    int timespan = Greedy::calculateTimespan(dependency_graph, result);
    bool valid_result = Greedy::checkScheduleValidity(dependency_graph, result);
    if (!valid_result) {
        std::cout << "INVALID RESULT!!!!!!!!!!!!!!!\n\n\n\n\n\n\n\n\n\n\n\n";
    }

    std::cout << timespan << ": ";
    for (auto value : result) {
        std::cout << value << " ";
    }
    std::cout << "\n";

    return 0;
}
