#include "greedy.hpp"

#include <algorithm>
#include <random>
#include <set>

#include "dependency_graph.hpp"

constexpr float dependent_delay_multiplier = 0.4;

int cascadedDependentsDelay(int id, const DependencyGraph& dependency_graph) {
    int cost = 0;
    auto jobs = dependency_graph.getJobs();
    auto precedence_delay = dependency_graph.getPrecedenceDelay();

    for (int dependent : jobs.at(id).getDependents()) {
        cost += precedence_delay[id - 1][dependent - 1];
        cost += cascadedDependentsDelay(dependent, dependency_graph);
    }

    return cost;
}

std::vector<std::pair<int, float>> constructInitialCandidateList(const DependencyGraph& dependency_graph) {
    std::vector<std::pair<int, float>> candidate_list;

    for (const auto& [id, job] : dependency_graph.getJobs()) {
        if (job.getDependencyCount() != 0) {
            continue;
        }

        auto sequence_setup_time = dependency_graph.getSequenceSetupTime()[id - 1];

        int sum = 0;
        int total_summed = 0;
        for (int i = 0; i < sequence_setup_time.size(); i++) {
            if (id == (i + 1)) {  // ids are always 1 value bigger since they start at 1
                continue;
            }

            sum += sequence_setup_time[i];
            total_summed++;
        }

        int cascaded_dependents_delay = cascadedDependentsDelay(id, dependency_graph);
        float choice_points = ((job.getProcessingTime() + ((float) sum / (float) total_summed)) / 2
                              ) -
                              (cascaded_dependents_delay * dependent_delay_multiplier);

        candidate_list.push_back({id, choice_points});
    }

    // sort from smallest to biggest
    std::sort(
        candidate_list.begin(), candidate_list.end(),
        [](const std::pair<int, float>& first, const std::pair<int, float>& second) {
            return first.second < second.second;
        }
    );

    return candidate_list;
}

std::vector<std::pair<int, float>> constructCandidateList(int previous_job, const std::set<int>& not_yet_completed_jobs, const DependencyGraph& dependency_graph, const std::map<int, int>& time_left_on_delay) {
    std::vector<std::pair<int, float>> candidate_list;

    for (int id : not_yet_completed_jobs) {
        if (
            dependency_graph.getJobs().at(id).getDependencyCount() != 0 &&
            !(time_left_on_delay.find(id) != time_left_on_delay.end() && time_left_on_delay.at(id) <= 0)
        ) {
            continue;
        }

        // int processing_time = dependency_graph.getJobs().at(id).getProcessingTime();
        int cascaded_dependents_delay = cascadedDependentsDelay(id, dependency_graph);
        int sequence_setup = dependency_graph.getSequenceSetupTime()[previous_job - 1][id - 1];

        float choice_points = sequence_setup - ((float) cascaded_dependents_delay * dependent_delay_multiplier);

        candidate_list.push_back({id, choice_points});
    }

    // sort from smallest to biggest
    std::sort(
        candidate_list.begin(), candidate_list.end(),
        [](const std::pair<int, float>& first, const std::pair<int, float>& second) {
            return first.second < second.second;
        }
    );

    return candidate_list;
}

int pickCandidate(const std::vector<std::pair<int, float>>& candidate_list, float alpha, unsigned int seed) {
    int upper_limit = static_cast<int>(alpha * (candidate_list.size() - 1));
    std::uniform_int_distribution<> uniform_distribution{0, upper_limit};

    std::mt19937 generator{seed};
    int candidate_index = uniform_distribution(generator);
    return candidate_list[candidate_index].first;
}

std::vector<int> Greedy::greedyRandomizedAdaptiveProcedure(const DependencyGraph& dependency_graph, float alpha, unsigned int seed) {
    // this is here mostly to reduce verbosity
    auto jobs = dependency_graph.getJobs();
    auto precedence_delay = dependency_graph.getPrecedenceDelay();
    auto sequence_setup_time = dependency_graph.getSequenceSetupTime();

    std::vector<int> solution;
    std::map<int, int> time_left_on_delay;

    std::set<int> not_yet_completed_jobs;
    for (const auto& [id, _] : jobs) {
        not_yet_completed_jobs.insert(id);
    }

    auto candidate_list = constructInitialCandidateList(dependency_graph);
    int candidate = pickCandidate(candidate_list, alpha, seed);

    solution.push_back(candidate);
    not_yet_completed_jobs.erase(candidate);

    for (auto dependent : jobs.at(candidate).getDependents()) {
        time_left_on_delay.emplace(
            dependent,
            precedence_delay[candidate - 1][dependent - 1]
        );
    }

    while (!not_yet_completed_jobs.empty()) {
        candidate_list = constructCandidateList(candidate, not_yet_completed_jobs, dependency_graph, time_left_on_delay);
        int new_candidate = pickCandidate(candidate_list, alpha, seed);
        time_left_on_delay.erase(new_candidate);

        for (auto& value : time_left_on_delay) {
            int time_to_remove = sequence_setup_time[candidate - 1][new_candidate - 1] + jobs.at(new_candidate).getProcessingTime();
            value.second -= time_to_remove;
        }

        for (auto dependent : jobs.at(new_candidate).getDependents()) {
            time_left_on_delay.emplace(
                dependent,
                precedence_delay[new_candidate - 1][dependent - 1]
            );
        }

        solution.push_back(new_candidate);
        not_yet_completed_jobs.erase(new_candidate);

        candidate = new_candidate;
    }

    return solution;
}

int Greedy::calculateTimespan(const DependencyGraph& dependency_graph, const std::vector<int>& schedule) {
    if (schedule.size() == 0) {
        return 0;
    }

    int timespan = 0;

    int previous_job = schedule[0];
    timespan += dependency_graph.getJobs().at(previous_job).getProcessingTime();

    for (auto job : schedule) {
        timespan += dependency_graph.getJobs().at(job).getProcessingTime();
        timespan += dependency_graph.getSequenceSetupTime()[previous_job - 1][job - 1];

        previous_job = job;
    }

    return timespan;
}
