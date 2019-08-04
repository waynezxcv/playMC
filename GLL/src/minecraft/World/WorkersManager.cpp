

#include "WorkersManager.hpp"

using namespace GLL;

WorkersManager* WorkersManager::sharedInstance() {
    static WorkersManager manager;
    return &manager;
}

void WorkersManager::setupWorkers() {
    mainWorker = new Worker();
    for (int i = 0; i < WORKER_COUNT; i ++) {
        this -> backgroundWorkers.push_back(std::make_shared<Worker>());
    }
}

Worker* WorkersManager::getMainWorker() {
    return mainWorker;
}

std::vector<std::shared_ptr<Worker>>&& WorkersManager::getbackgroundWorkers() {
    return std::move(this -> backgroundWorkers);
}

