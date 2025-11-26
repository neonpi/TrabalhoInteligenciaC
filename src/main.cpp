#include <filesystem>
#include <iostream>
#include "dependency_graph.hpp"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Too many parameters were supplied.\n";

        return 1;
    }

    std::filesystem::path instance_file_path{argv[1]};
    DependencyGraph dependency_graph{instance_file_path};

    return 0;
}
