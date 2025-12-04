#include "greedy.hpp"

#include <algorithm>
#include <iostream>
#include <random>
#include <set>
#include <stdexcept>

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
            (time_left_on_delay.find(id) == time_left_on_delay.end() || time_left_on_delay.at(id) > 0)
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
        throw std::invalid_argument("Empty schedule!\n");
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

bool Greedy::checkScheduleValidity(const DependencyGraph& dependency_graph, const std::vector<int>& schedule) {
    if (schedule.size() == 0) {
        throw std::invalid_argument("Empty schedule!\n");
    }

    // this is here mostly to reduce verbosity
    auto jobs = dependency_graph.getJobs();
    auto precedence_delay = dependency_graph.getPrecedenceDelay();
    auto sequence_setup_time = dependency_graph.getSequenceSetupTime();

    std::map<int, int> time_left_on_delay;
    std::set<int> can_be_added;
    for (const auto [id, job] : dependency_graph.getJobs()) {
        if (job.getDependencyCount() == 0) {
            can_be_added.insert(id);
        }
    }

    int previous_id = schedule[0];
    can_be_added.erase(previous_id);
    for (auto dependent_id : dependency_graph.getJobs().at(previous_id).getDependents()) {
        time_left_on_delay.emplace(dependent_id, dependency_graph.getPrecedenceDelay()[previous_id][dependent_id]);
    }

    for (auto id_iterator = std::next(schedule.cbegin(), 1); id_iterator != schedule.cend(); id_iterator++) {
        int id = *id_iterator;

        if (!can_be_added.contains(id)) {
            return false;
        }

        can_be_added.erase(id);

        // can't remove during loop, otherwise the loop will break
        std::vector<int> list_to_move;
        for (auto& value : time_left_on_delay) {
            int time_to_remove = sequence_setup_time[previous_id - 1][id - 1] + jobs.at(id).getProcessingTime();
            value.second -= time_to_remove;

            if (value.second <= 0) {
                list_to_move.push_back(value.first);
            }
        }

        for (auto value : list_to_move) {
            time_left_on_delay.erase(value);
            can_be_added.insert(value);
        }

        for (auto dependent_id : dependency_graph.getJobs().at(id).getDependents()) {
            if (precedence_delay[id - 1][dependent_id - 1] <= 0) {
                can_be_added.insert(dependent_id);
            } else {
                time_left_on_delay.emplace(dependent_id, dependency_graph.getPrecedenceDelay()[id - 1][dependent_id - 1]);
            }
        }

        previous_id = id;
    }

    if (can_be_added.size() != 0) {
        return false;
    }

    return true;
}

std::vector<int> Greedy::localSearch(const DependencyGraph& dependency_graph, float alpha, unsigned int seed) {
    auto best_result = Greedy::greedyRandomizedAdaptiveProcedure(dependency_graph, alpha, seed);
    auto best_timespan = Greedy::calculateTimespan(dependency_graph, best_result);

    // swaps two indices
    {
        bool improved = true;
        constexpr int max_iterations = 500;
        int iterations = 0;
        while (improved && iterations < max_iterations) {
            improved = false;

            int previous_best_timespan;
            for (int i = 0; i < best_result.size(); i++) {
                for (int j = 0; j < best_result.size(); j++) {
                    if (i == j) {
                        continue;
                    }

                    auto result = best_result;
                    std::swap(result[i], result[j]);
                    if (Greedy::checkScheduleValidity(dependency_graph, result)) {
                        auto timespan = Greedy::calculateTimespan(dependency_graph, result);

                        if (timespan < best_timespan) {
                            best_result = result;
                            best_timespan = timespan;

                            improved = true;
                            break;
                        }
                    }
                }

                if (best_timespan < previous_best_timespan) {
                    break;
                }
            }

            iterations++;
        }
    }

    // inserts
    {
        bool improved = true;
        constexpr int max_iterations = 500;
        int iterations = 0;
        while (improved && iterations < max_iterations) {
            improved = false;

            int previous_best_timespan;
            for (int i = 0; i < best_result.size(); i++) {
                for (int j = 0; j < best_result.size(); j++) {
                    if (i == j) {
                        continue;
                    }

                    auto result = best_result;
                    int value = result[i];
                    result.erase(result.begin() + i);
                    result.insert(result.begin() + j, value);
                    if (Greedy::checkScheduleValidity(dependency_graph, result)) {
                        auto timespan = Greedy::calculateTimespan(dependency_graph, result);

                        if (timespan < best_timespan) {
                            best_result = result;
                            best_timespan = timespan;

                            improved = true;
                            break;
                        }
                    }
                }

                if (best_timespan < previous_best_timespan) {
                    break;
                }
            }

            iterations++;
        }
    }

    return best_result;
}
