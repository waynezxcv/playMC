
#include "Chunk.hpp"
#include "Camera.hpp"


using namespace GLL;

#pragma mark -
Chunk::Chunk(const GLfloat& x, const GLfloat& z) :
location(glm::vec2{x,z})
{
    highestBlocks.setAll(0);
}


Chunk::~Chunk() {
    deleteMeshBuffers();
}


void Chunk::cleanUp() {
    std::lock_guard<std::mutex> lock(this -> sectionMapMutex);
    this -> sectionMap.clear();
}

#pragma mark -

void Chunk::setBlock(const BlockId& blockId, const GLfloat& x,  const GLfloat& y, const GLfloat& z) {
    std::lock_guard<std::mutex> lock(updateBlockMutex);
    GLuint sectionIndex = y / CHUNK_SIZE - 1;
    
    std::shared_ptr<ChunkSection> section = this -> getSection(sectionIndex);
    int bY = (int)y % (int)CHUNK_SIZE;
    if (section != nullptr) {
        section -> updateBlock(blockId, x, bY, z);
        int hight = y;
        if (y == highestBlocks.get(x, z)) {
            if (section != nullptr) {
                std::shared_ptr<ChunkBlock> highBlock = section -> getBlock(x, hight -- , z);
                while (highBlock != nullptr && highBlock -> getBlockData().blockId == BlockId_Air) {
                    highBlock = section -> getBlock(x, hight -- , z);
                }
            }
        }
        else if (y > highestBlocks.get(x, z)) {
            highestBlocks.get(x, z) = y;
        }
    }
}


std::shared_ptr<ChunkSection> Chunk::getSection(const int index) {
    
    std::lock_guard<std::mutex> lock(this -> sectionMapMutex);
    if (isOutOfBouds(index)) {
        return nullptr;
    }
    
    bool found = sectionMap.find(index) != sectionMap.end();
    if (found) {
        return sectionMap[index];
    }
    
    std::shared_ptr<ChunkSection> section = std::make_shared<ChunkSection>(this -> getLocation(),index);
    section -> setParentChunk(this);
    std::pair<int, std::shared_ptr<ChunkSection>> pair(index, section);
    this -> sectionMap.insert(pair);
    return section;
}


#pragma mark -

void Chunk::makeMeshes() {
    
    if (this -> hasMeshMade == true) {
        return;
    }
    
    for (int i = 0; i < CHUNK_SIZE; i ++) {
        
        sectionMapMutex.lock();
        std::shared_ptr<ChunkSection> section = this -> sectionMap[i];
        sectionMapMutex.unlock();
        
        section -> makeMeshes();
    }
    this -> hasMeshMade = true;
}

void Chunk::bufferMeshes() {
    
    if (this -> hasMeshBuffered == true) {
        return;
    }
    for (int i = 0; i < CHUNK_SIZE; i ++) {
        
        sectionMapMutex.lock();
        std::shared_ptr<ChunkSection> section = this -> sectionMap[i];
        sectionMapMutex.unlock();
        
        section -> bufferMeshes();
    }
    this -> hasMeshBuffered = true;
}

void Chunk::deleteMeshBuffers() {
    for (int i = 0; i < CHUNK_SIZE; i ++) {
        sectionMapMutex.lock();
        std::shared_ptr<ChunkSection> section = this -> sectionMap[i];
        sectionMapMutex.unlock();
        if (section != nullptr) {
            section -> deleteMeshBuffers();
        }
    }
}

void Chunk::drawChunk(Shader& shader, Camera* camera) {
#if !INSTANCE_DRAW_ENABLED
    if (this -> hasMeshMade == false) {
        this -> makeMeshes();
    }
    if (this -> hasMeshBuffered == false) {
        this -> bufferMeshes();
    }
    for (int i = 0; i <CHUNK_SIZE; i ++) {
        sectionMapMutex.lock();
        std::shared_ptr<ChunkSection> section = this -> sectionArray[i];
        sectionMapMutex.unlock();
        section -> drawSection(shader, camera);
    }
#endif
}

bool Chunk::isOutOfBouds(const int index) {
    if (index < 0 || index >= CHUNK_SIZE) {
        return true;
    }
    return false;
}

glm::vec2 Chunk::getLocation() const {
    return this -> location;
}

void Chunk::setLocation(const glm::vec2& location) {
    this -> location = location;
}


bool Chunk::getHasLoaded() {
    std::lock_guard<std::mutex> lock(this -> loadedMutex);
    bool hasLoaded = this -> hasLoaded;
    return hasLoaded;
}

void Chunk::load(WorldMapGenerator& generator) {
    std::lock_guard<std::mutex> lock(this -> loadedMutex);
    if (hasLoaded == true) {
        return;
    }
    generator.generateTerrainFor(*this);
    this -> hasLoaded = true;
}

int Chunk::getHeightAt(int x, int z) {
    return highestBlocks.get(x, z);
}

void Chunk::lockForReading() {
    this -> readingMutex.lock();
}


void Chunk::unlockForReading() {
    this -> readingMutex.unlock();
}
