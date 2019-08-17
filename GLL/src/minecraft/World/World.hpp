
#ifndef World_hpp
#define World_hpp

#include <stdio.h>
#include <vector>
#include "WorkersManager.hpp"
#include "MasterRender.hpp"
#include "ChunkManager.hpp"
#include "FPSCounter.hpp"
#include "Player.hpp"


namespace GLL {
    class World {
    public:
        std::shared_ptr<Camera> camera;
        
    public:
        World();
        ~World();
        void draw(std::shared_ptr<FrameBuffer> frameBuffer);
        MasterRender& getMasterRender();
        ChunkManager& getChunkManager();
        
    private:
        MasterRender masterRender;
        ChunkManager chunkManager;
        glm::vec3 playerSpawnPoint;
        WorkersManager workersManager;
        FPSCounter fpsCounter;

        int loadDistance = 2;
        std::atomic<int> inflightCount {0};
        std::mutex mainMutex;
        
    private:
        void setSpawnPoint();
        void loadChunks(std::shared_ptr<Camera> camera);
        void setupMeshInstances();
        
    };
}

#endif /* World_hpp */
