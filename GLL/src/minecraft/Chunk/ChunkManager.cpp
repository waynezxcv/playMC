
#include "ChunkManager.hpp"
#include "World.hpp"
#include "ChunkRender.hpp"


using namespace GLL;

ChunkManager::ChunkManager(ChunkRender* chunkRender, LiquidRender* liquidRender, FloraRender* floraRender) :
chunkRender(chunkRender),
liquidRender(liquidRender),
floraRender(floraRender),
worldGenerator(WorldMapGenerator()) {
    pthread_mutex_init(&chunkMapLock, NULL);
    pthread_mutex_init(&blocksLock, NULL);
    pthread_mutex_init(&instanceMeshLock, NULL);
}


ChunkManager::~ChunkManager() {
    chunkMap.clear();
    pthread_mutex_destroy(&chunkMapLock);
    pthread_mutex_destroy(&blocksLock);
    pthread_mutex_destroy(&instanceMeshLock);
}

bool ChunkManager::loadChunksIfNeeded(int x, int z) {
    for (int nx = -1; nx <= 1; nx ++) {
        for (int nz = -1; nz <= 1; nz ++) {
            this -> loadChunk(x + nx * CHUNK_SIZE, z + nz * CHUNK_SIZE) ;
        }
    }
    return true;
}

void ChunkManager::unloadChunksIfNeeded(const glm::vec3 &cameraPosition) {
    // 卸载附近的chunk
    for (int nx = -1; nx <= 1; nx ++) {
        for (int nz = -1; nz <= 1; nz ++) {
            this -> unloadChunk(cameraPosition.x + nx, cameraPosition.y + nz);
        }
    }
}

void ChunkManager::loadChunk(int x, int z) {
    
#if INSTANCE_DRAW_ENABLED
    // 1.获取xz坐标获取到chunk
    std::shared_ptr<Chunk> chunk = this -> getChunk(x, z);
    chunk -> load(worldGenerator);
    
    // 2. 遍历chunk中的sections
    for (int i = 0; i < CHUNK_SIZE; i ++) {
        std::shared_ptr<ChunkSection> section = chunk -> getSection(i);
        if (section != nullptr) {
            std::array<std::shared_ptr<ChunkBlock>, CHUNK_VOLUME> blocks = section -> getBlockArray();
            // 3.遍历section中的blocks
            for (int j = 0; j < CHUNK_VOLUME; j ++) {
                std::shared_ptr<ChunkBlock> block = blocks[j];
                pthread_mutex_lock(&blocksLock);
                this -> blocks.push_back(block);
                pthread_mutex_unlock(&blocksLock);
            }
        }
    }
    
    std::vector<std::shared_ptr<ChunkBlock>> blocks = this -> getBlocks();
    // 4. 遍历block，进行遮挡面剔除，并转换成isntanced渲染
    std::map<std::string, std::shared_ptr<InstanceMeshDrawable>> instanceMeshes;
    for (std::shared_ptr<ChunkBlock> block : blocks) {
        std::vector<InstanceMesh> meshes = block -> getInstanceMeshes();
        for (InstanceMesh mesh : meshes) {
            
            std::string key = this -> keyStringForMesh(mesh);
            pthread_mutex_lock(&instanceMeshLock);
            auto iterator = instanceMeshes.find(key);
            bool found = iterator != instanceMeshes.end();
            if (found) {
                std::pair<std::string, std::shared_ptr<InstanceMeshDrawable>> pair = *iterator;
                std::shared_ptr<InstanceMeshDrawable> drawable = pair.second;
                drawable -> addOffset(mesh.offset);
            }
            else {
                std::shared_ptr<InstanceMeshDrawable> drawable = std::make_shared<InstanceMeshDrawable>(mesh.blockData, mesh.direction);
                drawable -> addOffset(mesh.offset);
                std::pair<std::string, std::shared_ptr<InstanceMeshDrawable>> pair(key, drawable);
                instanceMeshes.insert(pair);
            }
            pthread_mutex_unlock(&instanceMeshLock);
        }
    }

    //5. 生成instanceMesDrawable对象
    GLuint count = 0;
    pthread_mutex_lock(&instanceMeshLock);
    for (auto it = instanceMeshes.begin(); it != instanceMeshes.end(); it ++) {
        std::pair<std::string, std::shared_ptr<InstanceMeshDrawable>> pair = *it;
        std::shared_ptr<InstanceMeshDrawable> drawable = pair.second;
        BlockShaderType shaderType = drawable -> getBlockData().shaderType;
        if (shaderType == BlockShaderType_Chunck) {
            this->chunkRender -> addDrawable(drawable);
        }
        else if (shaderType == BlockShaderType_Liquid) {
            this -> liquidRender -> addDrawable(drawable);
        }
        else if (shaderType == BlockShaderType_Flora) {
            this->floraRender -> addDrawable(drawable);
        }
        count += drawable->getOffsetsSize();
    }
    pthread_mutex_unlock(&instanceMeshLock);
#endif
}


void ChunkManager::unloadChunk(int x, int z) {
    if (chunkExistAt(x, z)) {
        pthread_mutex_lock(&chunkMapLock);
        chunkMap.erase(VectorXZ{x,z});
        pthread_mutex_unlock(&chunkMapLock);
    }
}

std::string ChunkManager::keyStringForMesh(InstanceMesh mesh) {
    return std::to_string(mesh.blockData.blockId) +
    std::to_string(mesh.blockData.meshType) +
    std::to_string( mesh.direction) +
    std::to_string(mesh.blockData.shaderType);
}


std::vector<std::shared_ptr<ChunkBlock>> ChunkManager::getBlocks() {
    pthread_mutex_lock(&blocksLock);
    std::vector<std::shared_ptr<ChunkBlock>> blocks = this -> blocks;
    pthread_mutex_unlock(&blocksLock);
    return blocks;
}

bool ChunkManager::chunkExistAt(int x, int z) {
    pthread_mutex_lock(&chunkMapLock);
    bool existed = chunkMap.find(VectorXZ{x,z}) != chunkMap.end();
    pthread_mutex_unlock(&chunkMapLock);
    return existed;
}


std::shared_ptr<Chunk> ChunkManager::getChunk(int x, int z) {
    VectorXZ key {x,z};
    if (this -> chunkExistAt(x, z) == false) {
        std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(x, z);
        if (chunk != nullptr) {
            pthread_mutex_lock(&chunkMapLock);
            this -> chunkMap.emplace(key, std::move(chunk));
            pthread_mutex_unlock(&chunkMapLock);
        }
    }
    pthread_mutex_lock(&chunkMapLock);
    std::shared_ptr<Chunk> chunk = this -> chunkMap[key];
    pthread_mutex_unlock(&chunkMapLock);
    return chunk;
}

bool ChunkManager::chunkHasLoadedAt(int x, int z) {
    if (this -> chunkExistAt(x, z) == false) {
        return false;
    }
    pthread_mutex_lock(&chunkMapLock);
    bool loaded = chunkMap.at(VectorXZ{x,z}) -> getHasLoaded();
    pthread_mutex_unlock(&chunkMapLock);
    return loaded;
}


WorldMapGenerator& ChunkManager::getWorldGenerator() {
    return worldGenerator;
}
