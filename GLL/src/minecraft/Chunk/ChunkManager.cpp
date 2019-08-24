
#include "ChunkManager.hpp"
#include "World.hpp"
#include "ChunkRender.hpp"
#include "Camera.hpp"

using namespace GLL;


namespace GLL {
    // 加载附近的4个chunk
    constexpr int RENDER_DISTANCE =  CHUNK_SIZE * 2;
}


#pragma mark - LifeCycle

ChunkManager::ChunkManager() {
    
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

#pragma mark - Chunk

void ChunkManager::traviesingChunks(std::function<void(std::shared_ptr<Chunk>)> callback) {
    std::lock_guard<std::mutex> lock(this -> chunkMapMutex);
    for (auto& chunk : chunkMap) {
        if (callback) {
            callback(chunk.second);
        }
    }
}

void ChunkManager::traviesingNeedRenderChunks(std::function<void(std::shared_ptr<Chunk>)> callback) {
    
    glm::vec3 cameraPositon = this -> camera -> getCameraPosition();
    int x = cameraPositon.x;
    int z = cameraPositon.z;
    
    VectorXZ xz = Chunk::normalizeChunkCoordination(x, z);
    x = xz.x;
    z = xz.z;
    
    std::lock_guard<std::mutex> lock(this -> chunkMapMutex);
    
    for (auto& one : chunkMap) {
        std::shared_ptr<Chunk> chunk = one.second;
        glm::vec2 chunkLocaiton = chunk -> getLocation();
        
        if (abs(chunkLocaiton.x - cameraPositon.x) > RENDER_DISTANCE ||
            abs(chunkLocaiton.y - cameraPositon.z) > RENDER_DISTANCE) {
            continue;
        }

        if (callback) {
            callback(chunk);
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
    chunk -> load(worldGenerator);
    
    // 3. 遍历chunk中的sections, 添加到block容器中
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
    return worldGenerator;
}

unsigned int ChunkManager::getChunksCount() {
    this -> chunkMapMutex.lock();
    auto size = chunkMap.size();
    this -> chunkMapMutex.unlock();
    return (unsigned int)size;
}
