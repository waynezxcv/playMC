
#include "Chunk.hpp"
#include "Camera.hpp"

using namespace GLL;



Chunk::Chunk(const GLfloat& x, const GLfloat& z) :
location(glm::vec2{x,z}),
hasLoaded(false) {}

Chunk::~Chunk() {
    std::lock_guard<std::mutex> lock(this -> sectionMapMutex);
    this -> sectionMap.clear();
}

void Chunk::setupSections() {
    if (hasLoaded == true) {
        return;
    }
    hasLoaded = true;
    std::shared_ptr<Chunk> shared = shared_from_this();
    std::weak_ptr<Chunk> weakSelf = shared;
    for (int i = 0; i < CHUNK_SIZE; i ++) {
        std::shared_ptr<ChunkSection> section = std::make_shared<ChunkSection>(weakSelf,i);
        section -> setupBlocks();
        std::lock_guard<std::mutex> lock(this -> sectionMapMutex);
        this -> sectionMap.emplace(i, std::move(section));
    }
}

void Chunk::setBlock(const BlockId& blockId, const GLfloat& x,  const GLfloat& y, const GLfloat& z) {
    GLuint sectionIndex = y / CHUNK_SIZE - 1;
    if (isOutOfBouds(sectionIndex) ) {
        return;
    }
    int bY = (int)y % (int)CHUNK_SIZE;
    std::lock_guard<std::mutex> lock(this -> sectionMapMutex);
    std::shared_ptr<ChunkSection> section = sectionMap[sectionIndex];
    section -> setBlock(blockId, x, bY, z);
}

bool Chunk::isOutOfBouds(const int index) {
    if (index < 0 || index >= CHUNK_SIZE) {
        return true;
    }
    return false;
}

void Chunk::load(WorldMapGenerator& generator) {
    generator.generateTerrainFor(*this);
}

void Chunk::traversingSections(std::function<void(std::shared_ptr<ChunkSection>)> callback) {
    std::lock_guard<std::mutex> lock(this -> sectionMapMutex);
    for (auto iterator = sectionMap.begin(); iterator != sectionMap.end(); ++ iterator ) {
        std::pair<int, std::shared_ptr<ChunkSection>> pair = *iterator;
        if (callback) {
            callback(pair.second);
        }
    }
}

std::shared_ptr<ChunkSection> Chunk::getSection(const int index) {
    if (isOutOfBouds(index)) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(this -> sectionMapMutex);
    return sectionMap[index];
}


bool Chunk::getHasLoaded() {
    return hasLoaded;
}

glm::vec2 Chunk::getLocation() const {
    return location;
}
