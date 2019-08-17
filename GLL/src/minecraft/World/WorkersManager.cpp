

#include "WorkersManager.hpp"

using namespace GLL;

WorkersManager::WorkersManager() {
    setupWorkers();
}

WorkersManager::~WorkersManager() {
    backgroundWorkers.clear();
}

void WorkersManager::setupWorkers() {
    for (int i = 0; i < WORKER_COUNT; i ++) {
        this -> backgroundWorkers.push_back(std::make_shared<Worker>());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

std::vector<std::shared_ptr<Worker>>&& WorkersManager::getbackgroundWorkers() {
    return std::move(this -> backgroundWorkers);
}

