#include "dependency_graph.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

DependencyGraph::DependencyGraph(std::filesystem::path instance_file_path) {
    if (!std::filesystem::is_regular_file(instance_file_path)) {
        throw std::invalid_argument{"Given file path doesn't point to a regular file.\n"};
    }

    std::ifstream file_reader{instance_file_path};
    std::string current_line;

    // Parse first line "R=[number]"
    std::getline(file_reader, current_line);
    std::string job_amount_string = current_line.substr(current_line.find("=") + 1);
    int job_amount = std::stoi(job_amount_string);

    for (int i = 0; i < job_amount; i++) {
        std::vector<int> precedence_delay;

        for (int j = 0; j < job_amount; j++) {
            if (i == j) {
                precedence_delay.push_back(-2);
            } else {
                precedence_delay.push_back(-1);
            }
        }

        this->precedence_delay.push_back(precedence_delay);
    }

    // Parse second line "Pi=([Number],[Number],...)"
    std::getline(file_reader, current_line);
    current_line = current_line.substr(current_line.find("(") + 1, current_line.size() - 1);  // gets only the comma separated numbers, removing both "(" and ")"

    for (int i = 1; i < job_amount; i++) {  // last value is handled separately
        auto comma_position = current_line.find(",");

        auto temp = current_line.substr(0, comma_position);
        this->jobs.emplace(i, Job{i, std::stoi(current_line.substr(0, comma_position))});

        current_line = current_line.substr(comma_position + 1);
    }

    // // handle last value
    this->jobs.emplace(job_amount, Job{job_amount, std::stoi(current_line)});

    // Parse next lines ("[number],[number],[number]") until "Sij="
    std::getline(file_reader, current_line);  // should be "A=", which cointains no useful information

    std::getline(file_reader, current_line);
    while (!current_line.contains("Sij=")) {
        auto first_comma_position = current_line.find(",");

        int depended_on = std::stoi(current_line.substr(0, first_comma_position));
        current_line = current_line.substr(first_comma_position + 1);

        auto second_comma_position = current_line.find(",");
        int dependent = std::stoi(current_line.substr(0, second_comma_position));
        int precedence_delay = std::stoi(current_line.substr(second_comma_position + 1));

        jobs.at(depended_on).addDependent(dependent);
        jobs.at(dependent).addDependency(depended_on);
        this->precedence_delay[depended_on - 1][dependent - 1] = precedence_delay;
        this->precedence_delay[dependent - 1][depended_on - 1] = -2;

        std::getline(file_reader, current_line);
    }

    // Parse next lines ("[number],[number],[number],...") until the end of the file
    // // no need to ignore "Sij=" since that will already have been read by the previous section
    while (std::getline(file_reader, current_line)) {
        std::vector<int> sequence_setup_time;

        for (int i = 1; i < job_amount; i++) {  // last value is handled separately
            auto comma_position = current_line.find(",");

            std::string setup_time = current_line.substr(0, comma_position);
            sequence_setup_time.push_back(std::stoi(setup_time));

            current_line = current_line.substr(comma_position + 1);
        }
        sequence_setup_time.push_back(std::stoi(current_line));

        this->sequence_setup_time.push_back(sequence_setup_time);
    }

    file_reader.close();
}

const std::map<int, Job>& DependencyGraph::getJobs() const {
    return this->jobs;
}

const std::vector<std::vector<int>>& DependencyGraph::getPrecedenceDelay() const {
    return this->precedence_delay;
}

const std::vector<std::vector<int>>& DependencyGraph::getSequenceSetupTime() const {
    return this->sequence_setup_time;
}

void DependencyGraph::exportGraph(std::filesystem::path output_file_path) const {
    std::ofstream file_writer{output_file_path};

    file_writer << "R=" << this->jobs.size() << "\n";

    file_writer << "Pi=(";
    file_writer << this->jobs.cbegin()->second.getProcessingTime();
    for (auto iterator = std::next(this->jobs.cbegin(), 1); iterator != this->jobs.end(); iterator++) {
        file_writer << "," << iterator->second.getProcessingTime();
    }
    file_writer << ")" << "\n";

    file_writer << "A=" << "\n";
    for (auto [id, job] : this->jobs) {
        if (job.getDependents().empty()) {
            continue;
        }

        for (auto dependent : job.getDependents()) {
            file_writer << id << "," << dependent << "," << this->precedence_delay[id - 1][dependent - 1] << "\n";
        }
    }

    file_writer << "Sij=" << "\n";
    for (auto setup_time : this->sequence_setup_time) {
        file_writer << *setup_time.cbegin();
        for (auto iterator = std::next(setup_time.cbegin(), 1); iterator != setup_time.cend(); iterator++) {
            file_writer << "," << *iterator;
        }
        file_writer << "\n";
    }

    file_writer.close();
}
