

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


void WorkersManager::doAsync(std::function<void()> operation) {
    int inflightIndex = inflightCount % WORKER_COUNT;
    std::shared_ptr<Worker> inflightWorker = getbackgroundWorkers()[inflightIndex];
    inflightWorker -> enqueue(operation);
    inflightCount = inflightCount + 1;
}


void WorkersManager::doAsync(std::function<void()> operation, std::function<void()> completion) {
    int inflightIndex = inflightCount % WORKER_COUNT;
    std::shared_ptr<Worker> inflightWorker = getbackgroundWorkers()[inflightIndex];
    inflightWorker -> enqueue([&]()->void {
        if (operation) {
            operation();
        }
        if (completion) {
            completion();
        }
    });
    inflightCount = inflightCount + 1;
}
