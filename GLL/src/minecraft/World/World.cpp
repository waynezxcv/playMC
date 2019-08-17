
#include <semaphore.h>
#include "World.hpp"
#include "RandomSingleton.hpp"
#include "Camera.hpp"
#include "InstanceMeshDrawable.hpp"

using namespace GLL;

World::World() :
masterRender(MasterRender(this)),
fpsCounter(FPSCounter{})
{
    GLL::CameraConfig config =  GLL::Camera::defaultCameraConfig();
    config.position = glm::vec3{0.0f, CHUNK_SIZE * CHUNK_SIZE / 4, CHUNK_SIZE * 2};
    config.movementSpeed = 50.0f;
    camera = std::make_shared<Camera>(config);
}


World::~World() {
    
}

void World::draw(std::shared_ptr<FrameBuffer> frameBuffer) {
    
    if (this -> camera == nullptr) {
        return;
    }
    // 加载 chunks
    this -> loadChunks(this -> camera);
    
    // 更新 chunks，看是否需要卸载一些chunks
    chunkManager.updateNeedRenderChunks(this -> camera);
    
    // 更新Meshes
    this -> setupMeshInstances();
    
    // 渲染
    this -> masterRender.draw(camera, frameBuffer);
    
    // 刷新FPS计算
    this -> fpsCounter.update();
}


void World::setupMeshInstances() {
    chunkManager.traviesingChunks([this](std::shared_ptr<Chunk> chunk) -> void {
        bool isMeshMade = chunk -> makeMeshIfNeeded();
        if (!isMeshMade) {
            return;
        }
        
        auto map = chunk -> getDrawableMap();
        for (auto one : map) {
            BlockShaderType shaderType =  one.second -> getBlockData().shaderType;
            if (shaderType == BlockShaderType_Chunck) {
                masterRender.chunkRender.addDrawableIfNeeded(one);
            }
            else if (shaderType == BlockShaderType_Liquid) {
                masterRender.liquidRender.addDrawableIfNeeded(one);
            }
            else if (shaderType == BlockShaderType_Flora) {
                masterRender.floraRender.addDrawableIfNeeded(one);
            }
        }
    });
}

MasterRender& World::getMasterRender() {
    return this -> masterRender;
}

ChunkManager& World::getChunkManager() {
    return this -> chunkManager;
}

#pragma mark - Load & Render

void World::setSpawnPoint() {
    double start = glfwGetTime();
    std::cout << "Searching for spawn...\n";
    
    int attempts = 0;
    int chunkX = -1;
    int chunkZ = -1;
    int blockX = 0;
    int blockZ = 0;
    int blockY = 0;
    auto h = chunkManager.getWorldGenerator().getMinimumSpawnHeight();
    
    while(blockY <= h) {
        chunkManager.unloadChunk(chunkX, chunkZ);
        chunkX = RandomSingleton::sharedInstance().intInRange(100, 200);
        chunkZ = RandomSingleton::sharedInstance().intInRange(100, 200);
        blockX = RandomSingleton::sharedInstance().intInRange(0, 15);
        blockZ = RandomSingleton::sharedInstance().intInRange(0, 15);
        chunkManager.loadChunk(chunkX, chunkZ);
        blockY = chunkManager.getChunk(chunkX, chunkZ) -> getHeightAt(blockX, blockZ);
        attempts++;
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
    std::cout<< "Spawn found! Attempts:"<< attempts << " Time Taken: " << glfwGetTime() - start << " seconds\n";
}

void World::loadChunks(std::shared_ptr<Camera> camera) {
    
    int inflightIndex = inflightCount % WORKER_COUNT;
    std::shared_ptr<Worker> inflightWorker = workersManager.getbackgroundWorkers()[inflightIndex];
    glm::vec3 cameraPosiiton = camera -> getCameraPosition();
    
    inflightWorker -> enqueue([this, cameraPosiiton]() {
        
        //        for (int x = 0; x < 1; ++x) {
        //            for (int z = 0; z < 1; ++z) {
        //                std::lock_guard<std::mutex> lock(this -> mainMutex);
        //                chunkManager.loadChunk(x, z);
        //            }
        //        }
        //
        
        bool isChunkLoaded = false;
        int cameraX = cameraPosiiton.x / CHUNK_SIZE;
        int cameraZ = cameraPosiiton.z / CHUNK_SIZE;
        
        for (int i = 0; i < loadDistance; i++) {
            
            int minX = std::max(cameraX  - i, 0);
            int minZ = std::max(cameraZ  - i, 0);
            
            int maxX = cameraX + i;
            int maxZ = cameraZ + i;
            
            for (int x = minX; x < maxX; ++x) {
                for (int z = minZ; z < maxZ; ++z) {
                    std::lock_guard<std::mutex> lock(this -> mainMutex);
                    isChunkLoaded = chunkManager.loadChunk(x, z);
                }
            }
            
            // 如果这个distance内的chunk已经load，则进入下次循环
            if (isChunkLoaded) {
                break;
            }
        }
        
        if (!isChunkLoaded) {
            loadDistance ++;
        }
        
        
        if (loadDistance >= RENDER_DISTANCE) {
            loadDistance = 2;
        }
        
    });
    
    inflightCount = inflightIndex + 1;
}

