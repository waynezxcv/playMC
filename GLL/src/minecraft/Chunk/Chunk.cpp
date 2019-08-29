
#include "Chunk.hpp"
#include "Camera.hpp"
#include "InstanceMeshDrawable.hpp"
#include "MasterRender.hpp"


using namespace GLL;


Chunk::Chunk(const GLfloat& x, const GLfloat& z) :
location(glm::vec2{x,z}) {}


Chunk::~Chunk() {
    this -> sectionMapMutex.lock();
    this -> sectionMap.clear();
    this -> sectionMapMutex.unlock();
}


void Chunk::setupSectionIfNeeded(int index) {
    
    if (sectionMap.find(index) == sectionMap.end()) {
        std::shared_ptr<Chunk> shared = shared_from_this();
        std::weak_ptr<Chunk> weakSelf = shared;
        std::shared_ptr<ChunkSection> section  = std::make_shared<ChunkSection>(weakSelf,index);
        section -> setupBlocks();
        sectionMap.emplace(index, std::move(section));
    }
    
    std::shared_ptr<ChunkSection> section = this -> sectionMap.at(index);
    this -> updateNearestChunkSections(section);
    
}

void Chunk::setBlock(const BlockId& blockId, GLfloat x,  GLfloat y, GLfloat z) {
    GLuint sectionIndex = y / CHUNK_SIZE;
    if (isOutOfBouds(sectionIndex) ) {
        return;
    }
    this -> sectionMapMutex.lock();
    this -> setupSectionIfNeeded(sectionIndex);
    std::shared_ptr<ChunkSection> section = this -> sectionMap.at(sectionIndex);
    int bY = (int)y % (int)CHUNK_SIZE;
    section -> setBlock(blockId, x, bY, z);
    this -> sectionMapMutex.unlock();
    this -> updateHighestBlocks(x, y, z);
}

void Chunk::updateNearestChunkSections(std::shared_ptr<ChunkSection> section) {
    
    GLuint sectionIndex = section -> getIndexOfParentChunkSections();
    if (sectionMap.find(sectionIndex - 1) != sectionMap.end()) {
        section -> downChunkSection = sectionMap.at(sectionIndex - 1);
    }
    if (sectionMap.find(sectionIndex + 1) != sectionMap.end()) {
        section -> upChunkSection = sectionMap.at(sectionIndex + 1);
    }
    std::shared_ptr<Chunk> leftChunk = this -> getLeftChunk();
    if (leftChunk != nullptr) {
        std::shared_ptr<ChunkSection> chunkSection = leftChunk -> getSection(sectionIndex);
        if (chunkSection != nullptr) {
            section -> leftChunkSection = chunkSection;
        }
    }
    std::shared_ptr<Chunk> rightChunk = this -> getRightChunk();
    if (rightChunk != nullptr) {
        std::shared_ptr<ChunkSection> chunkSection = rightChunk -> getSection(sectionIndex);
        if (chunkSection != nullptr) {
            section -> rightChunkSection = chunkSection;
        }
    }
    std::shared_ptr<Chunk> frontChunk = this -> getFrontChunk();
    if (frontChunk != nullptr) {
        std::shared_ptr<ChunkSection> chunkSection = frontChunk -> getSection(sectionIndex);
        if (chunkSection != nullptr) {
            section -> frontChunkSection = chunkSection;
        }
    }
    std::shared_ptr<Chunk> backChunk = this -> getBackChunk();
    if (backChunk != nullptr) {
        std::shared_ptr<ChunkSection> chunkSection = backChunk -> getSection(sectionIndex);
        if (chunkSection != nullptr) {
            section -> backChunkSection = chunkSection;
        }
    }
}

std::shared_ptr<ChunkBlock> Chunk::getBlock(int x, int y, int z)  {
    // 根据世界坐标y找到对应的index值
    GLuint sectionIndex = y / CHUNK_SIZE;
    if (isOutOfBouds(sectionIndex)) {
        return nullptr;
    }
    this -> sectionMapMutex.lock();
    this -> setupSectionIfNeeded(sectionIndex);
    int bY = (int)y % (int)CHUNK_SIZE;
    std::shared_ptr<ChunkBlock> block = this -> sectionMap.at(sectionIndex) -> getBlock(x, bY, z);
    this -> sectionMapMutex.unlock();
    return block;
}

void Chunk::updateHighestBlocks(int x, int y, int z) {
    std::lock_guard<std::mutex> lock(this -> highesetBlocksMutex);
    VectorXZ key {x,z};
    if (highestBlocks.find(key) == highestBlocks.end()) {
        highestBlocks.emplace(key,std::move(0));
    }
    if (y == highestBlocks.at(key)) {
        auto highBlock = getBlock(x, y, z);
        while (highBlock != nullptr && ! highBlock -> getBlockData().isOpaque) {
            highBlock =  getBlock(x, y -- , z);
        }
    }
    else if (y > highestBlocks.at(key)) {
        highestBlocks.emplace(key,std::move(y));
    }
}

int Chunk::getHeightAt(int x, int z) const {
    std::lock_guard<std::mutex> lock(this -> highesetBlocksMutex);
    VectorXZ key {x,z};
    bool found = this->highestBlocks.find(key) != this -> highestBlocks.end();
    if (!found) {
        return 0;
    }
    return highestBlocks.at(key);
}

bool Chunk::isOutOfBouds(const int index) {
    if (index < 0 || index >= CHUNK_SIZE) {
        return true;
    }
    return false;
}

void Chunk::load(WorldMapGenerator& generator) {
    std::shared_ptr<Chunk> sharedChunk =  shared_from_this();
    generator.generateTerrainFor(sharedChunk);
    hasLoaded = true;
}

void Chunk::traversingSections(std::function<void(std::shared_ptr<ChunkSection>)> callback) {
    this -> sectionMapMutex.lock();
    for (auto itr : sectionMap) {
        if (callback) {
            callback(itr.second);
        }
    }
    this -> sectionMapMutex.unlock();
}


std::shared_ptr<ChunkSection> Chunk::getSection(const int index) {
    if (isOutOfBouds(index)) {
        return nullptr;
    }
    this -> sectionMapMutex.lock();
    bool found = sectionMap.find(index) != sectionMap.end();
    std::shared_ptr<ChunkSection> section = nullptr;
    if (found) {
        section = sectionMap[index];
    }
    this -> sectionMapMutex.unlock();
    return section;
}

bool Chunk::getHasLoaded() {
    return hasLoaded;
}

glm::vec2 Chunk::getLocation() const {
    return location;
}

bool Chunk::makeMeshIfNeeded(MasterRender& masterRender) {
    
    if (this -> hasLoaded == false) {
        return false;
    }
    
    double startTime = glfwGetTime();
    
    std::map<std::string, std::vector<glm::vec3>> offsetsMap;
    this -> traversingSections([&](std::shared_ptr<ChunkSection> section) -> void {
        section -> travesingBlocks([&](std::shared_ptr<ChunkBlock> block) -> void {
            std::vector<std::shared_ptr<InstanceMesh>> meshes = this -> makeMeshesWithBlock(block);
            for (auto mesh : meshes) {
                std::string key = this -> hashWithInstanceMesh(mesh);
                std::shared_ptr<InstanceMeshDrawable> drawable = masterRender.getInstanceMeshDrawable(key);
                // create drawable if needed
                if (drawable == nullptr) {
                    drawable = std::make_shared<InstanceMeshDrawable>(mesh -> blockData, mesh -> direction);
                    std::pair<std::string, std::shared_ptr<InstanceMeshDrawable>> pair(key, drawable);
                    masterRender.insertInstanceMeshDrawableIfNeeded(std::move(pair));
                }
                if (offsetsMap.find(key) != offsetsMap.end()) {
                    offsetsMap[key].emplace_back(std::move(mesh -> offset));
                }
                else {
                    std::vector<glm::vec3> offsets {mesh -> offset};
                    std::pair<std::string, std::vector<glm::vec3>> pair(key, offsets);
                    offsetsMap.emplace(std::move(pair));
                }
            }
        });
    });
    
    startTime = glfwGetTime();
    
    for (auto& one : offsetsMap) {
        std::string key = one.first;
        std::vector<glm::vec3> offsets = one.second;
        std::shared_ptr<InstanceMeshDrawable> drawable = masterRender.getInstanceMeshDrawable(key);
        if (drawable != nullptr) {
            drawable -> addMeshOffsets(std::move(offsets));
        }
    }
    
    return true;
}

bool Chunk::unMakeMeshIfNeeded(MasterRender& masterRender) {
    //TODO:移除
    return true;
}


/*
 mesh表示一个block一个面的相关数据
 */
std::vector<std::shared_ptr<InstanceMesh>> Chunk::makeMeshesWithBlock(std::shared_ptr<ChunkBlock>block) {
    std::vector<std::shared_ptr<InstanceMesh>> meshes;
    BlockDataContent blockData = block -> getBlockData();
    if (blockData.blockId == BlockId_Air) {
        return meshes;
    }
    
    // 立方体
    if (blockData.meshType == BlockMeshType_Cube) {
        /*
         立方体做无用面剔除
         1. 位于立方体顶部的面，需要检查自己的上面的block是否是air类型
         2. 位于立方体底部的面，需要检查自己的下面的block是否是air类型
         3. 位于立方体左侧的面，需要检查自己的左边的block是否是air类型
         etc ...
         另外还需要在chunk层面，对相邻的两个section的顶部和底部做剔除
         */
        // 底部
        if (block -> downBlockId == BlockId_Air) {
            std::shared_ptr<InstanceMesh> mesh = std::make_shared<InstanceMesh>();
            mesh -> blockData = blockData;
            mesh -> direction =  ChunkMesh::ChunkMeshFaceDirection_NegativeY;
            mesh -> offset = block -> getBlockPositonInWorld();
            meshes.push_back(mesh);
        }
        
        // 顶部
        if ( block -> upBlockId == BlockId_Air ||
            block -> upBlockId == BlockId_Rose ||
            block -> upBlockId == BlockId_DeadShrub ||
            block -> upBlockId == BlockId_TallGrass) {
            std::shared_ptr<InstanceMesh> mesh = std::make_shared<InstanceMesh>();
            mesh -> blockData = blockData;
            mesh -> direction =  ChunkMesh::ChunkMeshFaceDirection_PositiveY;
            mesh -> offset = block -> getBlockPositonInWorld();
            meshes.push_back(mesh);
        }
        
        // 左边
        if ( block -> leftBlockId == BlockId_Air) {
            std::shared_ptr<InstanceMesh> mesh = std::make_shared<InstanceMesh>();
            mesh -> blockData = blockData;
            mesh -> direction =  ChunkMesh::ChunkMeshFaceDirection_NegativeX;
            mesh -> offset = block -> getBlockPositonInWorld();
            meshes.push_back(mesh);
        }
        // 右边
        if ( block -> rightBlockId == BlockId_Air) {
            std::shared_ptr<InstanceMesh> mesh = std::make_shared<InstanceMesh>();
            mesh -> blockData = blockData;
            mesh -> direction =  ChunkMesh::ChunkMeshFaceDirection_PositiveX;
            mesh -> offset = block -> getBlockPositonInWorld();
            meshes.push_back(mesh);
        }
        // 前面
        if ( block -> frontBlockId == BlockId_Air) {
            std::shared_ptr<InstanceMesh> mesh = std::make_shared<InstanceMesh>();
            mesh -> blockData = blockData;
            mesh -> direction =  ChunkMesh::ChunkMeshFaceDirection_PositiveZ;
            mesh -> offset = block -> getBlockPositonInWorld();
            meshes.push_back(mesh);
            
        }
        // 后面
        if (block -> backBlockId == BlockId_Air ) {
            std::shared_ptr<InstanceMesh> mesh = std::make_shared<InstanceMesh>();
            mesh -> blockData = blockData;
            mesh -> direction =  ChunkMesh::ChunkMeshFaceDirection_NegativeZ;
            mesh -> offset = block -> getBlockPositonInWorld();
            meshes.push_back(mesh);
        }
    }
    
    // X形不剔除了，都要显示
    else if (blockData.meshType == BlockMeshType_X) {
        
        std::shared_ptr<InstanceMesh> mesh1 = std::make_shared<InstanceMesh>();
        mesh1 -> blockData = blockData;
        mesh1 -> direction =  ChunkMesh::ChunkMeshFaceDirection_XZ;
        mesh1 ->offset = block -> getBlockPositonInWorld();
        meshes.push_back(mesh1);
        
        std::shared_ptr<InstanceMesh> mesh2 = std::make_shared<InstanceMesh>();
        mesh2 -> blockData = blockData;
        mesh2 -> direction =  ChunkMesh::ChunkMeshFaceDirection_ZX;
        mesh2 ->offset = block -> getBlockPositonInWorld();
        meshes.push_back(mesh2);
        
    }
    return meshes;
}

std::string Chunk::hashWithInstanceMesh(std::shared_ptr<InstanceMesh> mesh) {
    return std::to_string(mesh -> blockData.blockId) + std::to_string(mesh -> direction) ;
}


VectorXZ Chunk::normalizeChunkCoordination(int x, int z) {
    int bX = x % CHUNK_SIZE;
    int bZ = z % CHUNK_SIZE;
    return VectorXZ{bX * CHUNK_SIZE, bZ * CHUNK_SIZE};
}



std::shared_ptr<Chunk> Chunk::getLeftChunk() {
    std::shared_ptr<Chunk> sp = nullptr;
    if ( (sp = this -> leftChunk.lock()) ) {
        return sp;
    }
    return nullptr;
}


std::shared_ptr<Chunk> Chunk::getRightChunk() {
    std::shared_ptr<Chunk> sp = nullptr;
    if ( (sp = this -> rightChunk.lock()) ) {
        return sp;
    }
    return nullptr;
}


std::shared_ptr<Chunk> Chunk::getFrontChunk() {
    std::shared_ptr<Chunk> sp = nullptr;
    if ( (sp = this -> frontChunk.lock()) ) {
        return sp;
    }
    return nullptr;
}


std::shared_ptr<Chunk> Chunk::getBackChunk() {
    std::shared_ptr<Chunk> sp = nullptr;
    if ( (sp = this -> backChunk.lock()) ) {
        return sp;
    }
    return nullptr;
    
}
