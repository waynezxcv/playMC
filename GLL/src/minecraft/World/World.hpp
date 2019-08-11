
#ifndef World_hpp
#define World_hpp

#include <stdio.h>
#include <vector>
#include "WorkersManager.hpp"
#include "MasterRender.hpp"
#include "ChunkManager.hpp"
#include "FPSCounter.hpp"
#include "Player.hpp"
#include "InstanceMeshCollection.hpp"



namespace GLL {
    class World {
    public:
        World();
        ~World();
        
        void setCamera(Camera* camera);
        void draw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer);
        
        MasterRender& getMasterRender();
        ChunkManager& getChunkManager();
    private:
        MasterRender masterRender;
        ChunkManager chunkManager;
        glm::vec3 playerSpawnPoint;
        InstanceMeshCollection meshCollection;

        int loadDistance = 2;
        Camera* camera;
        FPSCounter fpsCounter;
        std::atomic<int> inflightCount {0};
        
    private:
        void setSpawnPoint();
        void loadChunks(Camera* camera);
        void setupMeshInstances();
    };
}

#endif /* World_hpp */
