
#include <semaphore.h>
#include "World.hpp"
#include "RandomSingleton.hpp"
#include "Camera.hpp"
#include "InstanceMeshDrawable.hpp"
#include "WorkersManager.hpp"


using namespace GLL;

MasterRender& World::getMasterRender() {
    return this -> masterRender;
}

ChunkManager& World::getChunkManager() {
    return this -> chunkManager;
}


#pragma mark - Life Cyle

World::World() :
fpsCounter(FPSCounter{}),
loadWorkerManager(WorkersManager(3)),
renderWorkerManager(WorkersManager(3))

{
    GLL::CameraConfig config =  GLL::Camera::defaultCameraConfig();
    config.position = glm::vec3{0.0f, CHUNK_SIZE * CHUNK_SIZE / 4, CHUNK_SIZE * 2};
    config.movementSpeed = 50.0f;
    camera = std::make_shared<Camera>(config);
    this -> chunkManager.camera = this -> camera;
}


World::~World() {
    
}

#pragma mark -

void World::draw(std::shared_ptr<FrameBuffer> frameBuffer) {
    // 加载 chunks
    this -> loadChunks(this -> camera);
    // 更新Meshes
    this -> makeMeshInstances();
    // 渲染
    this -> renderWorld(frameBuffer);
}


void World::loadChunks(std::shared_ptr<Camera> camera) {
    
    glm::vec3 cameraPosiiton = camera -> getCameraPosition();
    
    loadWorkerManager.doAsync([=]()-> void {
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
}

void World::makeMeshInstances() {
    // 以chunk为单位进行绘制
    renderWorkerManager.doAsync([=](){
        chunkManager.traviesingChunks(
                                      [&](std::shared_ptr<Chunk> chunk) -> void {
                                          chunk -> makeMeshIfNeeded(masterRender);
                                      },
                                      
                                      [&](std::shared_ptr<Chunk> chunk) -> void {
                                          chunk -> unMakeMeshIfNeeded(masterRender);
                                      }
                                      );
    },[=]() {
        this -> semaphore -> signal();
    });
}

void World::renderWorld(std::shared_ptr<FrameBuffer> frameBuffer) {
    this -> masterRender.draw(camera, frameBuffer);
    this -> fpsCounter.update();
    this -> semaphore -> wait();
}

