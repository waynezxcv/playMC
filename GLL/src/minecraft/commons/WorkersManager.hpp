
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
        static WorkersManager& sharedInstance();
        
        
        ~WorkersManager();
        
        void doAsync(std::function<void()> operation);
        void doAsync(std::function<void()> operation, std::function<void()> completion);
        
        
    private:
        WorkersManager();
        std::atomic<int> inflightCount {0};
        void setupWorkers();
        std::vector<std::shared_ptr<Worker>> backgroundWorkers;
        std::vector<std::shared_ptr<Worker>>&& getbackgroundWorkers();
    };
}

#endif /* WorkersManager_hpp */
