
#include "ChunkManager.hpp"
#include "World.hpp"
#include "ChunkRender.hpp"
#include "Camera.hpp"

using namespace GLL;

#pragma mark - LifeCycle

ChunkManager::ChunkManager() {
    
}

ChunkManager::~ChunkManager() {
    chunkMap.clear();
}

#pragma mark - Blocks

std::shared_ptr<ChunkBlock> ChunkManager::getBlock(int x,int y,int z) {
    std::lock_guard<std::mutex> lock(this -> chunkMapMutex);
    VectorXZ xz = this -> normalizeChunkCoordination(x, z);
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

VectorXZ ChunkManager::normalizeChunkCoordination(int x, int z) {
    int bX = x % CHUNK_SIZE;
    int bZ = z % CHUNK_SIZE;
    return VectorXZ{bX * CHUNK_SIZE, bZ * CHUNK_SIZE};
}


bool ChunkManager::loadChunk(int x, int z) {
    
    VectorXZ xz = normalizeChunkCoordination(x, z);
    x = xz.x;
    z = xz.z;
    
    // 1.获取xz坐标获取到chunk
    if (this -> chunkHasLoadedAt(x, z)) {
        return false;
    }
    std::cout<<">> start load chunk at : [ "<<x<<" , "<<z<<" ] , total count : "<<this->getChunksCount()<<std::endl;
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
    VectorXZ xz = normalizeChunkCoordination(x, z);
    x = xz.x;
    z = xz.z;
    if (chunkExistAt(x, z)) {
        this -> chunkMapMutex.lock();
        chunkMap.erase(VectorXZ{x,z});
        this -> chunkMapMutex.unlock();
        //        std::cout<<">> unload chunk at : [ "<<x<<" , "<<z<<" ]"<<std::endl;
    }
}

void ChunkManager::updateNeedRenderChunks(Camera* camera) {
    std::lock_guard<std::mutex> lock(this -> chunkMapMutex);
    for (auto iterator = chunkMap.begin(); iterator != chunkMap.end();) {
        VectorXZ xz = iterator -> first;
        std::shared_ptr<Chunk> chunk = iterator -> second;
        glm::vec3 cameraPosition = camera -> getCameraPosition();
        
        int cameraX = cameraPosition.x;
        int cameraZ = cameraPosition.z;
        int minX = (cameraX / CHUNK_SIZE) - RENDER_DISTANCE;
        int minZ = (cameraZ / CHUNK_SIZE) - RENDER_DISTANCE;
        int maxX = (cameraX / CHUNK_SIZE) + RENDER_DISTANCE;
        int maxZ = (cameraZ / CHUNK_SIZE) + RENDER_DISTANCE;
        auto location = chunk -> getLocation();
        
        if (minX > location.x || minZ > location.y || maxZ < location.y || maxX < location.x) {
            std::cout<<">> unload chunk at : [ "<<xz.x<<" , "<<xz.z<<" ]"<<std::endl;
            iterator = chunkMap.erase(iterator);
            continue;;
        }
        else {
            // need draws
            iterator++;
        }
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
