

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


ChunkSection::~ChunkSection() {
    
}

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
                this -> updateFacesX(block, x, y, z);
                this -> updateFacesY(block, x, y, z);
                this -> updateFacesZ(block, x, y, z);
#endif
                if (callback) {
                    callback(block);
                }
            }
        }
    }
}

void ChunkSection::updateFacesX(std::shared_ptr<ChunkBlock> block, int x, int y, int z) {
    if (x > 0 && x < CHUNK_SIZE - 1) {
        if (x > 0 && blockArray[x - 1][y][z] != nullptr) {
            block->leftBlockId = blockArray[x - 1][y][z] -> getBlockData().blockId;
        }
        if (x < CHUNK_SIZE - 1 && blockArray[x + 1][y][z] != nullptr) /* Face X+ */ {
            block->rightBlockId = blockArray[x + 1][y][z] -> getBlockData().blockId;
        }
    }
    else {
        
        if (x == 0) {
            // 右表面
            if (blockArray[x + 1][y][z] != nullptr) /* Face X+ */ {
                block -> rightBlockId = blockArray[x + 1][y][z] -> getBlockData().blockId;
            }
            // 左表面
            std::shared_ptr<ChunkSection> leftSection = this -> getLeftSection();
            if (leftSection == nullptr) {
                block -> leftBlockId = BlockId_Air;
            }
            else {
                std::shared_ptr<ChunkBlock> leftBlock = leftSection -> getBlock(CHUNK_SIZE - 1, y, z);
                if (leftBlock == nullptr) {
                    block -> leftBlockId = BlockId_Air;
                }
                else {
                    block -> leftBlockId = leftBlock -> getBlockData().blockId;
                }
            }
        }
        else if (x == CHUNK_SIZE - 1) {
            // 右表面
            std::shared_ptr<ChunkSection> rightSection = this -> getRightSection();
            if (rightSection == nullptr) {
                block -> rightBlockId = BlockId_Air;
            }
            else {
                std::shared_ptr<ChunkBlock> rightBlock = rightSection -> getBlock(0, y, z);
                if (rightBlock == nullptr) {
                    block -> rightBlockId = BlockId_Air;
                }
                else {
                    block -> rightBlockId = rightBlock -> getBlockData().blockId;
                }
            }
            //左表面
            if (blockArray[x - 1][y][z] != nullptr) {
                block->leftBlockId = blockArray[x - 1][y][z] -> getBlockData().blockId;
            }
        }
    }
}

void ChunkSection::updateFacesY(std::shared_ptr<ChunkBlock> block, int x, int y, int z) {
    if (y > 0 && y < CHUNK_SIZE - 1) {
        if (blockArray[x][y - 1][z] != nullptr) {
            block -> downBlockId = blockArray[x][y - 1][z] -> getBlockData().blockId;
        }
        if (blockArray[x][y + 1][z] != nullptr) {
            block -> upBlockId = blockArray[x][y + 1][z] -> getBlockData().blockId;
        }
    }
    else {
        if (y == 0) {
            // 上表面
            if (blockArray[x][y + 1][z] != nullptr) {
                block -> upBlockId = blockArray[x][y + 1][z] -> getBlockData().blockId;
            }
            // 下表面
            block -> downBlockId = BlockId_Count;
        }
        else if (y == CHUNK_SIZE - 1) {
            // 上表面
            std::shared_ptr<ChunkSection> upSection = this -> getUpSection();
            if (upSection == nullptr) {
                block -> upBlockId = BlockId_Count;
            }
            else {
                std::shared_ptr<ChunkBlock> upBlock = upSection -> getBlock(x, 0, z);
                if (upBlock == nullptr) {
                    block -> upBlockId = BlockId_Count;
                }
                else {
                    block -> upBlockId = upBlock -> getBlockData().blockId;
                }
            }
            // 下表面
            if (blockArray[x][y - 1][z] != nullptr) {
                block -> downBlockId = blockArray[x][y - 1][z] -> getBlockData().blockId;
            }
        }
    }
}

void ChunkSection::updateFacesZ(std::shared_ptr<ChunkBlock> block, int x, int y, int z) {
    if (z > 0 && z < CHUNK_SIZE - 1) {
        if (blockArray[x][y][z - 1] != nullptr) /* Face Z- */ {
            block -> backBlockId = blockArray[x][y][z - 1] -> getBlockData().blockId;
        }
        if (blockArray[x][y][z + 1] != nullptr) /* Face Z+ */ {
            block -> frontBlockId = blockArray[x][y][z + 1] -> getBlockData().blockId;
        }
    }
    else {
        if (z == 0) {
            // 后表面
            std::shared_ptr<ChunkSection> backSection = this -> getBackSection();
            if (backSection == nullptr) {
                block -> backBlockId = BlockId_Air;
            }
            else {
                std::shared_ptr<ChunkBlock> backBlock = backSection -> getBlock(x, y, CHUNK_SIZE - 1);
                if (backBlock == nullptr) {
                    block -> backBlockId = BlockId_Air;
                }
                else {
                    block -> backBlockId = backBlock -> getBlockData().blockId;
                }
            }
            //前表面
            if (blockArray[x][y][z + 1] != nullptr) /* Face Z+ */ {
                block -> frontBlockId = blockArray[x][y][z + 1] -> getBlockData().blockId;
            }
        }
        
        else if (z == CHUNK_SIZE - 1) {
            
            // 后表面
            if (blockArray[x][y][z - 1] != nullptr) /* Face Z- */ {
                block -> backBlockId = blockArray[x][y][z - 1] -> getBlockData().blockId;
            }
            // 前表面
            std::shared_ptr<ChunkSection> frontSection = this -> getFrontSection();
            if (frontSection == nullptr) {
                block -> frontBlockId = BlockId_Air;
            }
            else {
                std::shared_ptr<ChunkBlock> frontBlock = frontSection -> getBlock(x, y, 0);
                if (frontBlock == nullptr) {
                    block -> frontBlockId = BlockId_Air;
                }
                else {
                    block -> frontBlockId = frontBlock -> getBlockData().blockId;
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
    if ( (sp = this -> parentChunk.lock()) ) {
        return sp;
    }
    return nullptr;
}

void ChunkSection::setNeedUpdate() {
    this -> isNeedUpdate = true;
}

std::shared_ptr<ChunkSection> ChunkSection::getUpSection() {
    std::shared_ptr<ChunkSection> sp = nullptr;
    if ( (sp = this -> upChunkSection.lock()) ) {
        return sp;
    }
    return nullptr;
}

std::shared_ptr<ChunkSection> ChunkSection::getDownSection() {
    std::shared_ptr<ChunkSection> sp = nullptr;
    if ( (sp = this -> downChunkSection.lock()) ) {
        return sp;
    }
    return nullptr;
}


std::shared_ptr<ChunkSection> ChunkSection::getLeftSection() {
    std::shared_ptr<ChunkSection> sp = nullptr;
    if ( (sp = this -> leftChunkSection.lock()) ) {
        return sp;
    }
    return nullptr;
}

std::shared_ptr<ChunkSection> ChunkSection::getRightSection() {
    std::shared_ptr<ChunkSection> sp = nullptr;
    if ( (sp = this -> rightChunkSection.lock()) ) {
        return sp;
    }
    return nullptr;
}


std::shared_ptr<ChunkSection> ChunkSection::getFrontSection() {
    std::shared_ptr<ChunkSection> sp = nullptr;
    if ( (sp = this -> frontChunkSection.lock()) ) {
        return sp;
    }
    return nullptr;
}

std::shared_ptr<ChunkSection> ChunkSection::getBackSection() {
    std::shared_ptr<ChunkSection> sp = nullptr;
    if ( (sp = this -> backChunkSection.lock()) ) {
        return sp;
    }
    return nullptr;
    
}
