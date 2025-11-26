#include "job.hpp"

#include <algorithm>

Job::Job(int id, int processing_time): ID{id}, PROCESSING_TIME{processing_time} {};

int Job::getId() const {
    return this->ID;
};

int Job::getProcessingTime() const {
    return this->PROCESSING_TIME;
}

int Job::getDependencyCount() const {
    return this->dependencies.size();
}

const std::vector<int> Job::getDependencies() const {
    return this->dependencies;
}

void Job::addDependency(int dependency) {
    this->dependencies.push_back(dependency);
}

void Job::removeDependency(int dependency) {
    auto iterator = std::find(this->dependencies.cbegin(), this->dependencies.cend(), dependency);

    if (iterator != this->dependencies.end()) {
        this->dependencies.erase(iterator);
    }
}

const std::vector<int> Job::getDependents() const {
    return this->dependents;
}

void Job::addDependent(int dependent) {
    this->dependents.push_back(dependent);
}

void Job::removeDependent(int dependent) {
    auto iterator = std::find(this->dependents.cbegin(), this->dependents.cend(), dependent);

    if (iterator != this->dependents.end()) {
        this->dependents.erase(iterator);
    }
}
