
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
}

World::~World() {
}

void World::draw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) {
    if (this -> camera == nullptr) {
        return;
    }
    // 刷新FPS计算
    fpsCounter.update();

    // 加载 chunks
    this -> loadChunks(this -> camera);
    // 更新 chunks，看是否需要卸载一些chunks
    //chunkManager.updateNeedRenderChunks(camera);
    
    // 装载instance meshes
    setupMeshInstances();
    // 渲染
    masterRender.draw(camera, frameBuffer);
    
    ///清除instance meshes
    meshCollection.clear();
}

void World::setupMeshInstances() {
    chunkManager.traviesingChunks([this](std::shared_ptr<Chunk> chunk) -> void {
        chunk -> makeInstanceMeshes();
        std::vector<std::shared_ptr<InstanceMeshDrawable>> meshes = chunk -> getInstanceMeshes();
        for (auto& mesh : meshes) {
            BlockShaderType shaderType =  mesh -> getBlockData().shaderType;
            if (shaderType == BlockShaderType_Chunck) {
                this -> meshCollection.chunks.emplace_back(std::move(mesh));
            }
            else if (shaderType == BlockShaderType_Liquid) {
                this -> meshCollection.liquids.emplace_back(std::move(mesh));
            }
            else if (shaderType == BlockShaderType_Flora) {
                this -> meshCollection.floras.emplace_back(std::move(mesh));
            }
        }
    });
    
    for (auto chunk : meshCollection.chunks) {
        masterRender.chunkRender.addInstanceMesh(chunk);
    }
    
    for (auto liquid : meshCollection.liquids) {
        masterRender.liquidRender.addInstanceMesh(liquid);
    }
    
    for (auto flora : meshCollection.floras) {
        masterRender.floraRender.addInstanceMesh(flora);
    }
}


MasterRender& World::getMasterRender() {
    return this->masterRender;
}

ChunkManager& World::getChunkManager() {
    return this -> chunkManager;
}

void World::setCamera(Camera* camera) {
    this -> camera = camera;
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
            chunkManager.loadChunk(x, z);
        }
    }
    std::cout<< "Spawn found! Attempts:"<< attempts << " Time Taken: " << glfwGetTime() - start << " seconds\n";
}


void World::loadChunks(Camera* camera) {
    
    int inflightIndex = inflightCount % WORKER_COUNT;
    std::shared_ptr<Worker> inflightWorker = WorkersManager::sharedInstance()-> getbackgroundWorkers()[inflightIndex];
    glm::vec3 cameraPosiiton = camera -> getCameraPosition();
    
    inflightWorker -> enqueue([this, cameraPosiiton]() {
        
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

