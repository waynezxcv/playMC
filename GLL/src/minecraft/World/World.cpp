
#include <semaphore.h>
#include "World.hpp"
#include "RandomSingleton.hpp"
#include "Camera.hpp"
#include "InstanceMeshDrawable.hpp"

using namespace GLL;


#pragma mark - Life Cyle

World::World() :
fpsCounter(FPSCounter{})
{
    GLL::CameraConfig config =  GLL::Camera::defaultCameraConfig();
    config.position = glm::vec3{0.0f, CHUNK_SIZE * CHUNK_SIZE / 4, CHUNK_SIZE * 2};
    config.movementSpeed = 50.0f;
    camera = std::make_shared<Camera>(config);
    
    this -> chunkManager.camera = this -> camera;
    
    
    this -> setSpawnPoint();
}


World::~World() {
    
}

#pragma mark -

void World::setSpawnPoint() {
    
    workersManager.doAsync([&](){
        double start = glfwGetTime();
        std::cout << " >> searching for spawn...\n"<<std::endl;
        
        int attempts = 0;
        int chunkX = -1;
        int chunkZ = -1;
        int blockX = 0;
        int blockZ = 0;
        int blockY = 0;
        
        auto minSpawnHeight = chunkManager.getWorldGenerator().getMinimumSpawnHeight();
        
        while (blockY <= minSpawnHeight) {
            
            chunkManager.unloadChunk(chunkX, chunkZ);
            chunkX = RandomSingleton::sharedInstance().intInRange(100, 200);
            chunkZ = RandomSingleton::sharedInstance().intInRange(100, 200);
            blockX = RandomSingleton::sharedInstance().intInRange(0, 15);
            blockZ = RandomSingleton::sharedInstance().intInRange(0, 15);
            chunkManager.loadChunk(chunkX, chunkZ);
            
            blockY = chunkManager.getChunk(chunkX, chunkZ) -> getHeightAt(blockX, blockZ);
            attempts ++;
            
            std::cout<<" >> block y : [ "<<blockY<<" ]"<<std::endl;
        }
        
        int worldX = chunkX * CHUNK_SIZE + blockX;
        int worldZ = chunkZ * CHUNK_SIZE + blockZ;
        playerSpawnPoint = { worldX, blockY, worldZ };
        
        for (int x = worldX - 1; x <= worldX + 1; ++x) {
            for (int z = worldZ - 1; z < worldZ + 1; ++z) {
                std::lock_guard<std::mutex> lock(this -> mainMutex);
                chunkManager.loadChunk(x, z);
            }
        }
        
        std::cout<< " >> spawn found! Attempts:"<< attempts << " Time Taken: " << glfwGetTime() - start << " seconds\n"<<std::endl;
    });
}


void World::draw(std::shared_ptr<FrameBuffer> frameBuffer) {
    // 加载 chunks
    this -> loadChunks(this -> camera);
    
    // 更新Meshes
    this -> makeMeshInstances();
    
    // 渲染
    this -> render(frameBuffer);
}


void World::loadChunks(std::shared_ptr<Camera> camera) {
    
    //    glm::vec3 cameraPosiiton = camera -> getCameraPosition();
    //
    //    workersManager.doAsync([=]()-> void {
    //
    //        bool isChunkLoaded = false;
    //        int cameraX = cameraPosiiton.x / CHUNK_SIZE;
    //        int cameraZ = cameraPosiiton.z / CHUNK_SIZE;
    //
    //        for (int i = 0; i < loadDistance; i++) {
    //
    //            int minX = std::max(cameraX  - i, 0);
    //            int minZ = std::max(cameraZ  - i, 0);
    //
    //            int maxX = cameraX + i;
    //            int maxZ = cameraZ + i;
    //
    //
    //            for (int x = minX; x < maxX; ++x) {
    //                for (int z = minZ; z < maxZ; ++z) {
    //                    std::lock_guard<std::mutex> lock(this -> mainMutex);
    //                    isChunkLoaded = chunkManager.loadChunk(x, z);
    //                }
    //            }
    //
    //            // 如果这个distance内的chunk已经load，则进入下次循环
    //            if (isChunkLoaded) {
    //                break;
    //            }
    //        }
    //
    //        if (!isChunkLoaded) {
    //            loadDistance ++;
    //        }
    //
    //        if (loadDistance >= RENDER_DISTANCE) {
    //            loadDistance = 2;
    //        }
    //    });
}


void World::makeMeshInstances() {
    workersManager.doAsync([&]()->void {
        // 以chunk为单位进行绘制
        chunkManager.traviesingNeedRenderChunks([&](std::shared_ptr<Chunk> chunk) -> void {
            chunk -> makeMeshIfNeeded(masterRender);
        });
    });
}

void World::render(std::shared_ptr<FrameBuffer> frameBuffer) {
    this -> masterRender.draw(camera, frameBuffer);
    this -> fpsCounter.update();
}


MasterRender& World::getMasterRender() {
    return this -> masterRender;
}

ChunkManager& World::getChunkManager() {
    return this -> chunkManager;
}
