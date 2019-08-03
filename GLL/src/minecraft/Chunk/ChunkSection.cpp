

#include "ChunkSection.hpp"
#include "Chunk.hpp"
#include "Camera.hpp"


using namespace GLL;

constexpr GLfloat CHUNK_ORIGN_Y = - 2 * CHUNK_SIZE;

#pragma mark -
ChunkSection::ChunkSection(const glm::vec2& parentChunkPosition , const GLuint& index) :
indexInParentChunk(index),
parentChunkPosition(parentChunkPosition),
aabb({CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE})
{
    setupBlocks();
    positionInWorld = glm::vec3{parentChunkPosition.x, index * CHUNK_SIZE, parentChunkPosition.y };
    aabb.update({positionInWorld.x * CHUNK_SIZE, positionInWorld.y * CHUNK_SIZE, positionInWorld.z * CHUNK_SIZE});
}

glm::vec3 ChunkSection::getPositionInWorld() const {
    return positionInWorld;
}


ChunkSection::~ChunkSection() {
    deleteMeshBuffers();
}


void ChunkSection::setupBlocks() {
    std::lock_guard<std::mutex> lock(this -> traversingMutex);
    for (int i = 0; i < CHUNK_SIZE; i ++) {
        for (int j = 0; j < CHUNK_SIZE; j ++) {
            for (int k = 0; k < CHUNK_SIZE; k ++) {
                glm::vec3 blockPositionInWorld = glm::vec3{parentChunkPosition.x + i, indexInParentChunk * CHUNK_SIZE + j + CHUNK_ORIGN_Y, parentChunkPosition.y + k};
                std::shared_ptr<ChunkBlock> block = std::make_shared<ChunkBlock> (BlockId_Air, glm::vec3{i,j,k}, blockPositionInWorld);
                this -> setBlock(block);
            }
        }
    }
}

void ChunkSection::fillBlocks(const BlockId& blockId) {
    std::lock_guard<std::mutex> lock(this -> traversingMutex);
    for (int i = 0; i < CHUNK_SIZE; i ++) {
        for (int j = 0; j < CHUNK_SIZE; j ++) {
            for (int k = 0; k < CHUNK_SIZE; k ++) {
                std::shared_ptr<ChunkBlock> block = this -> blockArray[blockIndexFromPosistionInSection(i,j,k)];
                block -> updateBlockId(blockId);
            }
        }
    }
}

void ChunkSection::cleanUp() {
    this -> fillBlocks(BlockId_Air);
}

void ChunkSection::setParentChunk(Chunk* chunk) {
    this -> parentChunk = chunk;
}


Chunk* ChunkSection::getParentChunk() {
    return this -> parentChunk;
}

#pragma mark -

void ChunkSection::setBlock(std::shared_ptr<ChunkBlock> block) {
    std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
    if (isOutOfBounds(block -> getBlockPositionInSection().x, block -> getBlockPositionInSection().y, block -> getBlockPositionInSection().z)) {
        return;
    }
    block -> setParentSection(this);
    blockArray[blockIndexFromPosistionInSection(block -> getBlockPositionInSection().x, block -> getBlockPositionInSection().y, block -> getBlockPositionInSection().z)] = block;
}

void ChunkSection::updateBlock(const BlockId& blockId,const int x, const int y, const int z) {
    std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
    if (isOutOfBounds(x, y, z)) {
        return;
    }
    std::shared_ptr<ChunkBlock> block = blockArray[blockIndexFromPosistionInSection(x, y,z)];
    block -> updateBlockId(blockId);
}

std::shared_ptr<ChunkBlock> ChunkSection::getBlock(const int x, const int y, const int z) {
    std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
    if (isOutOfBounds(x, y, z)) {
        return nullptr;
    }
    std::shared_ptr<ChunkBlock> block = blockArray[blockIndexFromPosistionInSection(x,y,z)];
    return block;
}

#pragma mark -

void ChunkSection::makeMeshes() {
    std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
    if (this -> hasMeshMade == true) {
        return;
    }
    for (int i = 0; i < CHUNK_VOLUME; i ++) {
        std::shared_ptr<ChunkBlock> block = blockArray[i];
        block -> makeMeshes();
    }
    this -> hasMeshMade = true;
}

void ChunkSection::bufferMeshes() {
    std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
    if (this -> hasMeshBuffered == true) {
        return;
    }
    for (int i = 0; i < CHUNK_VOLUME; i ++) {
        std::shared_ptr<ChunkBlock> block = blockArray[i];
        block -> bufferMeshes();
    }
    this -> hasMeshBuffered = true;
}

void ChunkSection::deleteMeshBuffers() {
    std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
    for (int i = 0; i < CHUNK_VOLUME; i ++) {
        std::shared_ptr<ChunkBlock> block = blockArray[i];
        block -> deleteMeshBuffers();
    }
    this -> hasMeshMade = false;
    this -> hasMeshBuffered = false;
}

void ChunkSection::drawSection(Shader& shader, Camera* camera) {
    
    std::lock_guard<std::mutex> lock(this -> blockArrayMutex);
#if !INSTANCE_DRAW_ENABLED
#if AABB_TEST_ENABLED
    // 不在视角内的section不渲染
    if (camera -> getFrustum().isBoxInFrustum(this -> aabb)  == false) {
        return;
    }
#endif
    
    if (this -> hasMeshMade == false) {
        this -> makeMeshes();
    }

    if (this -> hasMeshBuffered == false) {
        this -> bufferMeshes();
    }

    for (int i = 0; i < CHUNK_VOLUME; i ++) {
        std::shared_ptr<ChunkBlock> block = blockArray[i];
        block -> drawBlock(shader);
    }
#endif
}


bool ChunkSection::isOutOfBounds(const int x, const int y, const int z) {
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


int ChunkSection::blockIndexFromPosistionInSection(const int x, const int y, const int z) {
    return y * CHUNK_AREA + z * CHUNK_SIZE + x;
}


GLuint ChunkSection::getIndexInParentChunk() const {
    return indexInParentChunk;
}


std::array<std::shared_ptr<ChunkBlock>, CHUNK_VOLUME>& ChunkSection::getBlockArray() {
    return blockArray;
}
