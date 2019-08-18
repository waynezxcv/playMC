

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
                std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
                this -> blockArray[i][j][k] = std::move(block);
            }
        }
    }
}


void ChunkSection::travesingBlocks(std::function<void(std::shared_ptr<ChunkBlock>)> callback) {
    
    std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
    
    for (int x = 0; x < CHUNK_SIZE; x ++) {
        for (int y = 0; y < CHUNK_SIZE; y ++) {
            for (int z = 0; z < CHUNK_SIZE; z ++) {
                std::shared_ptr<ChunkBlock> block = this -> blockArray[x][y][z];
#if CUSTOM_CULL_FACE_ENABLED
                if (x > 0 && blockArray[x - 1][y][z] != nullptr) {
                    block->leftBlockId = blockArray[x - 1][y][z] -> getBlockData().blockId;
                }
                if(x < CHUNK_SIZE - 1 && blockArray[x + 1][y][z] != nullptr) /* Face X+ */ {
                    block->rightBlockId = blockArray[x + 1][y][z] -> getBlockData().blockId;
                }
                
                if(y > 0 && blockArray[x][y - 1][z] != nullptr) /* Face Y- */ {
                    block -> downBlockId = blockArray[x][y - 1][z] -> getBlockData().blockId;
                }
                
                if(y < CHUNK_SIZE - 1  && blockArray[x][y + 1][z] != nullptr) /* Face Y+ */ {
                    block -> upBlockId = blockArray[x][y + 1][z] -> getBlockData().blockId;
                }
                
                if(z > 0 && blockArray[x][y][z - 1] != nullptr) /* Face Z- */ {
                    block -> backBlockId = blockArray[x][y][z - 1] -> getBlockData().blockId;
                }
                
                if(z < CHUNK_SIZE - 1 &&blockArray[x][y][z + 1] != nullptr) /* Face Z+ */ {
                    block -> frontBlockId = blockArray[x][y][z + 1] -> getBlockData().blockId;
                }
#endif
                if (callback) {
                    callback(block);
                }
            }
        }
    }
}

void ChunkSection::setBlock(const BlockId& blockId,const int& x, const int& y, const int& z) {
    if (isOutOfBounds(x, y, z)) {
        return;
    }
    std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
    std::shared_ptr<ChunkBlock> block = this -> blockArray[x][y][z];
    block -> updateBlockId(blockId);
}


std::shared_ptr<ChunkBlock> ChunkSection::getBlock(const int& x, const int& y, const int& z) {
    std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
    if (isOutOfBounds(x, y, z)) {
        return nullptr;
    }
    return this -> blockArray[x][y][z];
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

std::shared_ptr<Chunk> ChunkSection::getParentChunk() {
    std::shared_ptr<Chunk> sp = nullptr;
    if ((sp = this -> parentChunk.lock())) {
        return sp;
    }
    else {
        return nullptr;
    }
}

void ChunkSection::setNeedUpdate() {
    this -> isNeedUpdate = true;
}
