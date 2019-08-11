

#include "ChunkSection.hpp"
#include "Chunk.hpp"

using namespace GLL;

constexpr GLfloat CHUNK_ORIGN_Y = - 2 * CHUNK_SIZE;

ChunkSection::ChunkSection(std::weak_ptr<Chunk> parentChunk, const GLuint& index) :
parentChunk(parentChunk),
indexOfParentChunkSections(index),
hasLoaded(false),
aabb({CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE})
{
    if (std::shared_ptr<Chunk> sp = parentChunk.lock()) {
        parentChunkPosition = sp -> getLocation();
    }
    positionInWorld = glm::vec3{ parentChunkPosition.x, index * CHUNK_SIZE, parentChunkPosition.y };
    aabb.update({positionInWorld.x * CHUNK_SIZE, positionInWorld.y * CHUNK_SIZE, positionInWorld.z * CHUNK_SIZE});
}

ChunkSection::~ChunkSection() {}


void ChunkSection::setupBlocks() {
    
    if (hasLoaded == true) {
        return;
    }
    hasLoaded = true;
    
    std::shared_ptr<ChunkSection> shared = shared_from_this();
    std::weak_ptr<ChunkSection> weakSelf = shared;
    
    for (int i = 0; i < CHUNK_SIZE; i ++) {
        for (int j = 0; j < CHUNK_SIZE; j ++) {
            for (int k = 0; k < CHUNK_SIZE; k ++) {
                glm::vec3 blockPositionInWorld = glm::vec3{parentChunkPosition.x + i, indexOfParentChunkSections * CHUNK_SIZE + j + CHUNK_ORIGN_Y, parentChunkPosition.y + k};
                std::shared_ptr<ChunkBlock> block = std::make_shared<ChunkBlock> (weakSelf, BlockId_Air, glm::vec3{i,j,k}, blockPositionInWorld);
                auto index = getBlockIndexWithBlockPosition(block -> getBlockPositionInSection().x, block -> getBlockPositionInSection().y, block -> getBlockPositionInSection().z);
                std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
                this -> blockArray.at(index) = std::move(block);
            }
        }
    }
}

void ChunkSection::travesingBlocks(std::function<void(std::shared_ptr<ChunkBlock>)> callback) {
    std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
    for (auto block : blockArray) {
        if (callback) {
            callback(block);
        }
    }
}

void ChunkSection::setBlock(const BlockId& blockId,const int& x, const int& y, const int& z) {
    if (isOutOfBounds(x, y, z)) {
        return;
    }
    std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
    auto index = getBlockIndexWithBlockPosition(x, y, z);
    std::shared_ptr<ChunkBlock> block = this -> blockArray.at(index);
    block -> updateBlockId(blockId);
}


std::shared_ptr<ChunkBlock> ChunkSection::getBlock(const int& x, const int& y, const int& z) {
    std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
    if (isOutOfBounds(x, y, z)) {
        return nullptr;
    }
    auto index = getBlockIndexWithBlockPosition(x, y, z);
    std::shared_ptr<ChunkBlock> block = this -> blockArray.at(index);
    return block;
}

glm::vec2 ChunkSection::getLocation() const {
    return positionInWorld;
}

GLuint ChunkSection::getIndexOfParentChunkSections() const {
    return indexOfParentChunkSections;
}

bool ChunkSection::isOutOfBounds(const int& x, const int& y, const int& z) {
    if (x < 0 || x >= CHUNK_SIZE) {
        return true;
    }
    if (y < 0 || y >= CHUNK_SIZE) {
        return true;
    }
    if (z < 0 || z >= CHUNK_SIZE) {
        return true;
    }
    return false;
}

int ChunkSection::getBlockIndexWithBlockPosition(const int& x, const int& y, const int& z) const {
    return y * CHUNK_AREA + z * CHUNK_SIZE + x;
}

std::shared_ptr<Chunk> ChunkSection::getParentChunk() {
    std::shared_ptr<Chunk> sp = nullptr;
    if ((sp = this -> parentChunk.lock())) {
        return sp;
    }
    else {
        return nullptr;
    }
}
