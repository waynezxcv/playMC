
#ifndef WorkersManager_hpp
#define WorkersManager_hpp

#include <stdio.h>
#include <vector>
#include "NonCopyable.hpp"
#include "Worker.hpp"

namespace  {
    constexpr GLuint WORKER_COUNT = 3;
}

namespace GLL {
    class WorkersManager : public NonCopyable {
    public:
        static WorkersManager* sharedInstance();
        Worker* getMainWorker();
        
        std::vector<std::shared_ptr<Worker>>&& getbackgroundWorkers();
        ~WorkersManager() {
            delete mainWorker;
        }
        
    private:
        WorkersManager() {
            setupWorkers();
        }
        void setupWorkers();
        Worker* mainWorker;
        std::vector<std::shared_ptr<Worker>> backgroundWorkers;
    };
}

#endif /* WorkersManager_hpp */
