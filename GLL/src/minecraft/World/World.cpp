
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
    ChunkManager::sharedInstance() -> camera = this -> camera;
}


World::~World() {
    
}


#pragma mark -

void World::draw(std::shared_ptr<FrameBuffer> frameBuffer) {
    // 加载 chunks
    this -> loadChunks(this -> camera);
    // 更新Meshes
    this -> makeMeshInstances(this -> camera);
    // 渲染
    this -> renderWorld(frameBuffer);
}


void World::loadChunks(std::shared_ptr<Camera> camera) {
    loadWorkerManager.doAsync([=]()-> void {
        glm::vec3 cameraPosiiton = camera -> getCameraPosition();
        int minX =  cameraPosiiton.x -  RENDER_DISTANCE;
        int maxX = cameraPosiiton.x +  RENDER_DISTANCE;
        int maxZ = cameraPosiiton.z +  RENDER_DISTANCE;
        int minZ = cameraPosiiton.z - RENDER_DISTANCE;
        for (int x = minX; x < maxX; ++x) {
            for (int z = minZ; z < maxZ; ++z) {
                std::lock_guard<std::mutex> lock(this -> mainMutex);
                ChunkManager::sharedInstance() -> loadChunk(x, z);
            }
        }
    });
}


void World::makeMeshInstances(std::shared_ptr<Camera> camera) {
    
    auto makeMesh= [&](std::shared_ptr<Chunk> chunk) -> void {
        chunk -> makeMeshIfNeeded(masterRender, camera);
    };
    
    auto unMakeMesh = [&](std::shared_ptr<Chunk> chunk) -> void {
        chunk -> unMakeMeshIfNeeded(masterRender);
    };
    
    auto completion = [=]() {
        this -> semaphore -> signal();
    };
    
    auto traviesingChunks = [=](){
        ChunkManager::sharedInstance() -> traviesingChunks(makeMesh,unMakeMesh);
    };
    
    // 以chunk为单位进行绘制
    renderWorkerManager.doAsync(traviesingChunks, completion);
}


void World::renderWorld(std::shared_ptr<FrameBuffer> frameBuffer) {
    this -> masterRender.draw(camera, frameBuffer);
    this -> fpsCounter.update();
    this -> semaphore -> wait();
}

