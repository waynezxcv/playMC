
#ifndef World_hpp
#define World_hpp

#include <stdio.h>
#include <vector>
#include <memory>

#include "WorkersManager.hpp"
#include "MasterRender.hpp"
#include "ChunkManager.hpp"
#include "FPSCounter.hpp"
#include "Player.hpp"
#include "Semaphore.hpp"



namespace GLL {
    
    class World {
    public:
        std::shared_ptr<Camera> camera;
        
    public:
        
        World();
        ~World();
        
        void draw(std::shared_ptr<FrameBuffer> frameBuffer);
        MasterRender& getMasterRender();
        
    private:
        std::shared_ptr<Semaphore> semaphore = std::make_shared<Semaphore>(0);
        MasterRender masterRender;
        glm::vec3 playerSpawnPoint;
        FPSCounter fpsCounter;
        std::mutex mainMutex;
        WorkersManager loadWorkerManager;
        WorkersManager renderWorkerManager;
    private:
        void loadChunks(std::shared_ptr<Camera> camera);
        void renderWorld(std::shared_ptr<FrameBuffer> frameBuffer);
        void makeMeshInstances(std::shared_ptr<Camera> camera);
    };
}

#endif /* World_hpp */
