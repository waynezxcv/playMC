
#include "ChunkManager.hpp"
#include "World.hpp"
#include "ChunkRender.hpp"
#include "Camera.hpp"

using namespace GLL;


#pragma mark - LifeCycle

ChunkManager::ChunkManager() :
worldGenerator(std::make_unique<WorldMapGenerator>())
{
}

ChunkManager::~ChunkManager() {
    chunkMap.clear();
}

#pragma mark - Blocks

std::shared_ptr<ChunkBlock> ChunkManager::getBlock(int x,int y,int z) {
    std::lock_guard<std::mutex> lock(this -> chunkMapMutex);
    VectorXZ xz = Chunk::normalizeChunkCoordination(x, z);
    if (chunkMap.find(xz) == chunkMap.end()) {
        return nullptr;
    }
    auto chunk = chunkMap[xz];
    return chunk -> getBlock(x, y, z);
}

std::vector<std::shared_ptr<ChunkBlock>> ChunkManager::getBlocks() {
    std::lock_guard<std::mutex> lock(this -> blocksMutex);
    return this -> blocks;
}


void ChunkManager::travesingBlocks(std::function<void(std::shared_ptr<ChunkBlock>)> callback) {
    std::lock_guard<std::mutex> lock(this -> blocksMutex);
    for (auto& block : blocks) {
        if (callback) {
            callback(block);
        }
    }
}

#pragma mark - Section

std::shared_ptr<ChunkSection> ChunkManager::getSection(const int& x,const int& z,const int& sectionIndex) {
    std::lock_guard<std::mutex> lock(this -> chunkMapMutex);
    VectorXZ xz = Chunk::normalizeChunkCoordination(x, z);
    if (chunkMap.find(xz) == chunkMap.end()) {
        return nullptr;
    }
    auto chunk = chunkMap[xz];
    return chunk -> getSection(sectionIndex);
}


#pragma mark - Chunk

void ChunkManager::traviesingChunks(std::function<void(std::shared_ptr<Chunk>)> renderChunks, std::function<void(std::shared_ptr<Chunk>)> unRenderChunks) {
    glm::vec3 cameraPositon = this -> camera -> getCameraPosition();
    std::lock_guard<std::mutex> lock(this -> chunkMapMutex);
    for (auto& one : chunkMap) {
        std::shared_ptr<Chunk> chunk = one.second;
        glm::vec2 chunkLocaiton = chunk -> getLocation();
        if (abs(chunkLocaiton.x - cameraPositon.x) > RENDER_DISTANCE ||
            abs(chunkLocaiton.y - cameraPositon.z) > RENDER_DISTANCE ) {
            if (unRenderChunks) {
                unRenderChunks(chunk);
            }
            continue;
        }
        if (renderChunks) {
            renderChunks(chunk);
        }
    }
}

bool ChunkManager::loadChunk(int x, int z) {
    VectorXZ xz = Chunk::normalizeChunkCoordination(x, z);
    x = xz.x;
    z = xz.z;
    
    // 1.获取xz坐标获取到chunk
    if (this -> chunkHasLoadedAt(x, z)) {
        return false;
    }
    std::cout<<">> load chunk at : [ "<<x<<" , "<<z<<" ] , total loaded chunk count : [ "<<this->getChunksCount()<<" ]"<<std::endl;
    
    // 2.生成地图
    std::shared_ptr<Chunk> chunk = this -> getChunk(x, z);
    
    // 3.设置相邻的chunk
    this -> updateNearestChunks();
    
    // 4. 加载生成类型
    chunk -> load(this -> getWorldGenerator());
    
    // 5. 遍历chunk中的sections, 添加到block容器中
    chunk->traversingSections([&](std::shared_ptr<ChunkSection> section) -> void {
        section -> travesingBlocks([&](std::shared_ptr<ChunkBlock> block) -> void {
            std::lock_guard<std::mutex> lock(this -> blocksMutex);
            this -> blocks.emplace_back(std::move(block));
        });
    });
    return true;
}


void ChunkManager::unloadChunk(int x, int z) {
    VectorXZ xz = Chunk::normalizeChunkCoordination(x, z);
    x = xz.x;
    z = xz.z;
    if (chunkExistAt(x, z)) {
        std::cout<<">> unload chunk at : [ "<<x<<" , "<<z<<" ] , total loaded chunk count : [ "<<this->getChunksCount()<<" ]"<<std::endl;
        this -> chunkMapMutex.lock();
        chunkMap.erase(VectorXZ{x,z});
        this -> chunkMapMutex.unlock();
    }
}


void ChunkManager::updateNearestChunks() {
    
    std::lock_guard<std::mutex> lock(this -> chunkMapMutex);
    for (auto& one : chunkMap) {
        
        std::shared_ptr<Chunk> chunk = one.second;
        glm::vec2 chunkLocaiton = chunk -> getLocation();
        std::shared_ptr<Chunk> leftChunk = nullptr;
        std::shared_ptr<Chunk> rightChunk = nullptr;
        std::shared_ptr<Chunk> frontChunk = nullptr;
        std::shared_ptr<Chunk> backChunk = nullptr;
        
        VectorXZ leftKey = {(int)chunkLocaiton.x  - CHUNK_SIZE , (int)chunkLocaiton.y};
        VectorXZ rightKey = {(int)chunkLocaiton.x  + CHUNK_SIZE , (int)chunkLocaiton.y};
        VectorXZ frontKey = {(int)chunkLocaiton.x  , (int)chunkLocaiton.y + CHUNK_SIZE};
        VectorXZ backKey = {(int)chunkLocaiton.x   , (int)chunkLocaiton.y - CHUNK_SIZE};
        
        if (chunkMap.find(leftKey) != chunkMap.end()) {
            leftChunk = chunkMap.at(leftKey);
        }
        if (chunkMap.find(rightKey) != chunkMap.end()) {
            rightChunk = chunkMap.at(rightKey);
        }
        if (chunkMap.find(frontKey) != chunkMap.end()) {
            frontChunk = chunkMap.at(frontKey);
        }
        if (chunkMap.find(backKey) != chunkMap.end()) {
            backChunk = chunkMap.at(backKey);
        }
        
        chunk -> leftChunk = leftChunk;
        chunk -> rightChunk = rightChunk;
        chunk -> frontChunk = frontChunk;
        chunk -> backChunk = backChunk;
    }
}

bool ChunkManager::chunkExistAt(int x, int z) {
    this -> chunkMapMutex.lock();
    bool existed = chunkMap.find(VectorXZ{x,z}) != chunkMap.end();
    this -> chunkMapMutex.unlock();
    return existed;
}


std::shared_ptr<Chunk> ChunkManager::getChunk(int x, int z) {
    VectorXZ key {x,z};
    if (this -> chunkExistAt(x, z) == false) {
        std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(x, z);
        if (chunk != nullptr) {
            this -> chunkMapMutex.lock();
            this -> chunkMap.emplace(key, std::move(chunk));
            this -> chunkMapMutex.unlock();
        }
    }
    this -> chunkMapMutex.lock();
    std::shared_ptr<Chunk> chunk = this -> chunkMap.at(key);
    this -> chunkMapMutex.unlock();
    
    return chunk;
}

bool ChunkManager::chunkHasLoadedAt(int x, int z) {
    if (this -> chunkExistAt(x, z) == false) {
        return false;
    }
    this -> chunkMapMutex.lock();
    bool loaded = chunkMap.at(VectorXZ{x,z}) -> getHasLoaded();
    this -> chunkMapMutex.unlock();
    return loaded;
}

WorldMapGenerator& ChunkManager::getWorldGenerator() {
    return *this->worldGenerator;
}

unsigned int ChunkManager::getChunksCount() {
    this -> chunkMapMutex.lock();
    auto size = chunkMap.size();
    this -> chunkMapMutex.unlock();
    return (unsigned int)size;
}
