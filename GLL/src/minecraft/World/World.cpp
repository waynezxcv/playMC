
#include "World.hpp"
#include "RandomSingleton.hpp"
#include <semaphore.h>
#include "Camera.hpp"

using namespace GLL;

World::World() :
masterRender(MasterRender(this)),
chunkManager(ChunkManager( &masterRender.chunkRender , &masterRender.liquidRender, &masterRender.floraRender )),
fpsCounter(FPSCounter()){}

World::~World() {
}

void World::draw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) {
    if (this -> camera == nullptr) {
        return;
    }
    
    int inflightIndex = inflightCount % WORKER_COUNT;
    std::shared_ptr<Worker> inflightWorker = WorkersManager::sharedInstance()-> getbackgroundWorkers()[inflightIndex];
    inflightWorker -> enqueue([&]() {
        chunkManager.loadChunksIfNeeded(this -> camera->getCameraPosition().x, this -> camera->getCameraPosition().z);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    });
    inflightCount = inflightIndex + 1;
    
    masterRender.draw(camera, frameBuffer);
    fpsCounter.update();
}

MasterRender& World::getMasterRender() {
    return this->masterRender;
}

ChunkManager& World::getChunkManager() {
    return this -> chunkManager;
}

void World::setCamera(Camera* camera) {
    this -> camera = camera;
    this -> camera -> cameraDidUpdated = [&]()->void {
        this -> cameraDidUpdatedHandle();
    };
}

void World::cameraDidUpdatedHandle() {
}
