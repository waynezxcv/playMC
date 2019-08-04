
#include "ChunkManager.hpp"
#include "World.hpp"
#include "ChunkRender.hpp"


using namespace GLL;

ChunkManager::ChunkManager(ChunkRender* chunkRender, ChunkRender* liquidRender, ChunkRender* floraRender) :
chunkRender(chunkRender),
liquidRender(liquidRender),
floraRender(floraRender),
worldGenerator(WorldMapGenerator()) {
    pthread_mutex_init(&chunkMapLock, NULL);
    pthread_mutex_init(&blocksLock, NULL);
    pthread_mutex_init(&instanceLock, NULL);
}


ChunkManager::~ChunkManager() {
    chunkMap.clear();
    pthread_mutex_destroy(&chunkMapLock);
    pthread_mutex_destroy(&blocksLock);
    pthread_mutex_destroy(&instanceLock);
}

void ChunkManager::loadChunksIfNeeded(int x, int z) {
    if (x % CHUNK_SIZE == 0 && z % CHUNK_SIZE == 0) {
        this -> intenalLoadChunk(x, z);
    }
    else if (x % CHUNK_SIZE == 0 && z % CHUNK_SIZE != 0) {
        int finalZ = z / CHUNK_SIZE;
        this -> intenalLoadChunk(x, finalZ * CHUNK_SIZE);
    }
    else if (x % CHUNK_SIZE != 0 && z % CHUNK_SIZE == 0) {
        int finalX = x / CHUNK_SIZE;
        this -> intenalLoadChunk(finalX * CHUNK_SIZE, z);
    }
}

void ChunkManager::intenalLoadChunk(int x, int z) {
    for (int nx = -1; nx <= 1; nx ++) {
        for (int nz = -1; nz <= 1; nz ++) {
            this -> loadChunk(x + nx * CHUNK_SIZE, z + nz * CHUNK_SIZE);
        }
    }
}

void ChunkManager::unloadChunksIfNeeded(const glm::vec3 &cameraPosition) {
    for (int nx = -1; nx <= 1; nx ++) {
        for (int nz = -1; nz <= 1; nz ++) {
            this -> unloadChunk(cameraPosition.x + nx, cameraPosition.y + nz);
        }
    }
}

bool ChunkManager::loadChunk(int x, int z) {
    
    // 1.获取xz坐标获取到chunk
    if (this -> chunkExistAt(x, z) || this -> chunkHasLoadedAt(x, z)) {
        return false;
    }
    std::cout<<">> start load chunk at : [ "<<x<<" , "<<z<<" ]"<<std::endl;
    
    // 2.生成地图
    std::shared_ptr<Chunk> chunk = this -> getChunk(x, z);
    chunk -> load(worldGenerator);
    
    // 3. 遍历chunk中的sections
    chunk->traversingSections([&](std::shared_ptr<ChunkSection> section) -> void {
        section -> travesingBlocks([&](std::shared_ptr<ChunkBlock> block) -> void {
            pthread_mutex_lock(&blocksLock);
            this -> blocks.emplace_back(std::move(block));
            pthread_mutex_unlock(&blocksLock);
        });
    });
    
    
    // 4. 遍历block，进行遮挡面剔除，并转换成isntanced渲染
    std::vector<std::shared_ptr<ChunkBlock>> blocks = this -> getBlocks();
    for (std::shared_ptr<ChunkBlock> block : blocks) {
        std::vector<InstanceMesh> meshes = block -> getInstanceMeshes();
        for (InstanceMesh mesh : meshes) {
            pthread_mutex_lock(&instanceLock);
            std::string key = this -> keyStringForMesh(mesh);
            auto iterator = this -> instanceMeshDrawables.find(key);
            bool found = iterator != this -> instanceMeshDrawables.end();
            pthread_mutex_unlock(&instanceLock);
            if (found) {
                std::pair<std::string, std::shared_ptr<InstanceMeshDrawable>> pair = *iterator;
                std::shared_ptr<InstanceMeshDrawable> drawable = pair.second;
                drawable -> addOffset(mesh.offset);
            }
            else {
                std::shared_ptr<InstanceMeshDrawable> drawable = std::make_shared<InstanceMeshDrawable>(mesh.blockData, mesh.direction);
                drawable -> addOffset(mesh.offset);
                std::pair<std::string, std::shared_ptr<InstanceMeshDrawable>> pair(key, drawable);
                pthread_mutex_lock(&instanceLock);
                this -> instanceMeshDrawables.insert(pair);
                pthread_mutex_unlock(&instanceLock);
            }
        }
    }
    
    // 5. 生成instanceMesDrawable对象
    pthread_mutex_lock(&instanceLock);
    for (auto it = this -> instanceMeshDrawables.begin(); it !=  this -> instanceMeshDrawables.end(); it ++) {
        std::pair<std::string, std::shared_ptr<InstanceMeshDrawable>> pair = *it;
        std::shared_ptr<InstanceMeshDrawable> drawable = pair.second;
        BlockShaderType shaderType = drawable -> getBlockData().shaderType;
        if (shaderType == BlockShaderType_Chunck) {
            chunkRender -> addInstanceDrawablesIfNeeded(pair);
        }
        else if (shaderType == BlockShaderType_Liquid) {
            liquidRender -> addInstanceDrawablesIfNeeded(pair);
        }
        else if (shaderType == BlockShaderType_Flora) {
            floraRender -> addInstanceDrawablesIfNeeded(pair);
        }
    }
    pthread_mutex_unlock(&instanceLock);
    return true;
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
        chunk -> setupSections();
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
