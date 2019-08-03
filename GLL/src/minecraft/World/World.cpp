
#include "World.hpp"
#include "RandomSingleton.hpp"
#include <semaphore.h>



using namespace GLL;

namespace  {
    constexpr GLuint WORKER_COUNT = 3;
}

World::World() :
masterRender(MasterRender(this)),
chunkManager(ChunkManager( &masterRender.chunkRender , &masterRender.liquidRender, &masterRender.floraRender )),
fpsCounter(FPSCounter())
{
    setupWokers();
}

World::~World() {
    this -> workers.clear();
}

void World::setupWokers() {
    for (int i = 0; i < WORKER_COUNT; i ++) {
        this -> workers.push_back(std::make_shared<Worker>());
    }
}

void World::draw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) {
    int inflightIndex = inflightCount % WORKER_COUNT;
    std::shared_ptr<Worker> inflightWorker = this -> workers[inflightIndex];
    inflightWorker -> enqueue([this, camera, frameBuffer]() {
        chunkManager.loadChunksIfNeeded(camera -> getCameraPosition().x, camera -> getCameraPosition().z);
    });
    masterRender.draw(camera, frameBuffer);
    inflightCount = inflightIndex + 1;
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
}
