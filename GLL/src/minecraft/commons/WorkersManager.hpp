
#ifndef WorkersManager_hpp
#define WorkersManager_hpp

#include <stdio.h>
#include <vector>
#include "NonCopyable.hpp"
#include "Worker.hpp"

namespace GLL {
    class WorkersManager : public NonCopyable {
    public:
        explicit WorkersManager(const unsigned int& workersCount);
        WorkersManager(WorkersManager&& rhs);

        ~WorkersManager();
        void doAsync(std::function<void()> operation);
        void doAsync(std::function<void()> operation, std::function<void()> completion);
        
        
        
    private:
        unsigned workerCount = 0;
        std::atomic<int> inflightCount {0};
        std::vector<std::shared_ptr<Worker>> backgroundWorkers;

    private:
        void setupWorkers();
        std::vector<std::shared_ptr<Worker>>&& getbackgroundWorkers();
    };
}

#endif /* WorkersManager_hpp */
