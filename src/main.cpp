#include <chrono>
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

    auto start_time = std::chrono::high_resolution_clock::now();
    auto result = Greedy::localSearch(dependency_graph, 0.3, 3);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    int timespan = Greedy::calculateTimespan(dependency_graph, result);
    std::cout << timespan << " (" << duration.count() << "ms)" << ": ";
    for (auto value : result) {
        std::cout << value << " ";
    }
    std::cout << "\n";

    return 0;
}
