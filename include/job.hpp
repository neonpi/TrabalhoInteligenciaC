#ifndef __JOB_HPP__
#define __JOB_HPP__

#include <vector>

class Job {
    public:
        Job(int id, int processing_time);

        int getId() const;
        int getProcessingTime() const;

        int getDependencyCount() const;
        const std::vector<int> getDependencies() const;
        void addDependency(int dependency);
        void removeDependency(int dependency);

        const std::vector<int> getDependents() const;
        void addDependent(int dependent);
        void removeDependent(int dependent);

    private:
        const int ID;
        const int PROCESSING_TIME;

        std::vector<int> dependencies;
        std::vector<int> dependents;
};

#endif
