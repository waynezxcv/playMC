

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
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

Worker* WorkersManager::getMainWorker() {
    return mainWorker;
}


std::vector<std::shared_ptr<Worker>>&& WorkersManager::getbackgroundWorkers() {
    return std::move(this -> backgroundWorkers);
}

