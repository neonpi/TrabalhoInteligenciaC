#ifndef __DEPENDENCY_GRAPH_HPP__
#define __DEPENDENCY_GRAPH_HPP__

#include <filesystem>
#include <map>
#include "job.hpp"

class DependencyGraph {
    public:
        DependencyGraph(std::filesystem::path instance_file_path);

        const std::map<int, Job>& getJobs() const;
        const std::vector<std::vector<int>>& getPrecedenceDelay() const;
        const std::vector<std::vector<int>>& getSequenceSetupTime() const;

        // used for testing purposes
        void exportGraph(std::filesystem::path output_file_path) const;

    private:
        std::map<int, Job> jobs;
        std::vector<std::vector<int>> precedence_delay;
        std::vector<std::vector<int>> sequence_setup_time;
};

#endif
