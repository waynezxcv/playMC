
#include "Chunk.hpp"
#include "Camera.hpp"
#include "InstanceMeshDrawable.hpp"

using namespace GLL;


Chunk::Chunk(const GLfloat& x, const GLfloat& z) :
location(glm::vec2{x,z}) {}


Chunk::~Chunk() {
    std::lock_guard<std::mutex> lock(this -> sectionMapMutex);
    this -> sectionMap.clear();
}

void Chunk::setupSectionIfNeeded(int index) {
    if (sectionMap.find(index) == sectionMap.end()) {
        std::shared_ptr<Chunk> shared = shared_from_this();
        std::weak_ptr<Chunk> weakSelf = shared;
        std::shared_ptr<ChunkSection> section  = std::make_shared<ChunkSection>(weakSelf,index);
        section -> setupBlocks();
        sectionMap.emplace(index, std::move(section));
    }
}

void Chunk::setBlock(const BlockId& blockId, GLfloat x,  GLfloat y, GLfloat z) {
    GLuint sectionIndex = y / CHUNK_SIZE;
    if (isOutOfBouds(sectionIndex) ) {
        return;
    }
    this -> sectionMapMutex.lock();
    this -> setupSectionIfNeeded(sectionIndex);
    this -> sectionMap.at(sectionIndex) ->  setBlock(blockId, x, y, z);
    this -> sectionMapMutex.unlock();
    this -> updateHighestBlocks(x, y, z);
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


std::shared_ptr<ChunkBlock> Chunk::getBlock(int x, int y, int z)  {
    // 根据世界坐标y找到对应的index值
    GLuint sectionIndex = y / CHUNK_SIZE;
    if (isOutOfBouds(sectionIndex)) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(this -> sectionMapMutex);
    this -> setupSectionIfNeeded(sectionIndex);
    int bY = (int)y % (int)CHUNK_SIZE;
    return this -> sectionMap.at(sectionIndex) -> getBlock(x, bY, z);
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
    std::lock_guard<std::mutex> lock(this -> sectionMapMutex);
    for (auto itr : sectionMap) {
        if (callback) {
            callback(itr.second);
        }
    }
}

std::shared_ptr<ChunkSection> Chunk::getSection(const int index) {
    if (isOutOfBouds(index)) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(this -> sectionMapMutex);
    return sectionMap[index];
}

bool Chunk::getHasLoaded() {
    return hasLoaded;
}

glm::vec2 Chunk::getLocation() const {
    return location;
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

void Chunk::makeInstanceMeshes() {
    if (this -> hasMeshesMade) {
        return;
    }
    this -> traversingSections([this](std::shared_ptr<ChunkSection> section) -> void {
        section -> travesingBlocks([this](std::shared_ptr<ChunkBlock> block) -> void {
            InstanceMesh mesh = this -> meshWithBlock(block);
            std::shared_ptr<InstanceMeshDrawable> drawable = std::make_shared<InstanceMeshDrawable>(mesh.blockData, mesh.direction);
            this -> instanceMeshes.emplace_back(std::move(drawable));
        });
    });
    this -> hasMeshesMade = true;
}


InstanceMesh Chunk::meshWithBlock(std::shared_ptr<ChunkBlock>block) {
    
    BlockDataContent blockData = block -> getBlockData();
    InstanceMesh mesh;
    mesh.blockData = blockData;
    
    // 空气block当成一个占位用的block，不做任何渲染
    if (blockData.blockId == BlockId_Air) {
        return mesh;
    }
    return mesh;

    
    //
    //    // 立方体
    //    if (blockData.meshType == BlockMeshType_Cube) {
    //        /*
    //         立方体做无用面剔除
    //         1. 位于立方体顶部的面，需要检查自己的上面的block是否是air类型
    //         2. 位于立方体底部的面，需要检查自己的下面的block是否是air类型
    //         3. 位于立方体左侧的面，需要检查自己的左边的block是否是air类型
    //         etc ...
    //         另外还需要在chunk层面，对相邻的两个section的顶部和底部做剔除
    //         */
    //        // 底部
    //        BlockId down = this -> getDownBlockId();
    //        if (down == BlockId_Air) {
    //            InstanceMesh mesh;
    //            mesh.blockData = blockData;
    //            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_NegativeY;
    //            mesh.offset = blockPositonInWorld;
    //            instanceMeshes.push_back(mesh);
    //        }
    //
    //        // 顶部
    //        BlockId up = this -> getUpBlockId();
    //        if (up == BlockId_Air) {
    //
    //            InstanceMesh mesh;
    //            mesh.blockData = blockData;
    //            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_PositiveY;
    //            mesh.offset = blockPositonInWorld;
    //            instanceMeshes.push_back(mesh);
    //        }
    //
    //        // 左边
    //        BlockId left = this -> getLeftBlockId();
    //        if (left == BlockId_Air) {
    //            InstanceMesh mesh;
    //            mesh.blockData = blockData;
    //            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_NegativeX;
    //            mesh.offset = blockPositonInWorld;
    //            instanceMeshes.push_back(mesh);
    //        }
    //
    //        // 右边
    //        BlockId right = this -> getRightBlockId();
    //        if (right== BlockId_Air) {
    //            InstanceMesh mesh;
    //            mesh.blockData = blockData;
    //            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_PositiveX;
    //            mesh.offset = blockPositonInWorld;
    //            instanceMeshes.push_back(mesh);
    //        }
    //
    //        // 前面
    //        BlockId front = this -> getFrontBlockId();
    //        if (front == BlockId_Air) {
    //            InstanceMesh mesh;
    //            mesh.blockData = blockData;
    //            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_PositiveZ;
    //            mesh.offset = blockPositonInWorld;
    //            instanceMeshes.push_back(mesh);
    //        }
    //
    //        // 后面
    //        BlockId back = this -> getBackBlockId();
    //        if (back == BlockId_Air ) {
    //            InstanceMesh mesh;
    //            mesh.blockData = blockData;
    //            mesh.direction = ChunkMesh::ChunkMeshFaceDirection_NegativeZ;
    //            mesh.offset = blockPositonInWorld;
    //            instanceMeshes.push_back(mesh);
    //        }
    //    }
    //
    //    // X形不剔除了，都要显示
    //    else if (blockData.meshType == BlockMeshType_X) {
    //        InstanceMesh mesh1;
    //        mesh1.blockData = blockData;
    //        mesh1.direction =  ChunkMesh::ChunkMeshFaceDirection_XZ;
    //        mesh1.offset = blockPositonInWorld;
    //        instanceMeshes.push_back(mesh1);
    //
    //        InstanceMesh mesh2;
    //        mesh2.blockData = blockData;
    //        mesh2.direction =  ChunkMesh::ChunkMeshFaceDirection_ZX;
    //        mesh2.offset = blockPositonInWorld;
    //        instanceMeshes.push_back(mesh2);
    //    }
}



void Chunk::deleteInstanceMeshes() {
    instanceMeshes.clear();
    this -> hasMeshesMade = false;
}

bool Chunk::getHasInstanceMeshesMade() const {
    return hasMeshesMade;
}

std::vector<std::shared_ptr<InstanceMeshDrawable>>& Chunk::getInstanceMeshes() {
    return instanceMeshes;
}

std::string Chunk::keyStringForMesh(InstanceMesh mesh) {
    return std::to_string(mesh.blockData.blockId) +
    std::to_string(mesh.blockData.meshType) +
    std::to_string( mesh.direction) +
    std::to_string(mesh.blockData.shaderType);
}


//
//
//
//
//
//BlockId ChunkBlock::getUpBlockId() {
//
//    glm::vec3 positionInSection = this -> getBlockPositionInSection();
//    glm::vec3 destPosition = glm::vec3{positionInSection.x, positionInSection.y + 1, positionInSection.z};
//    std::shared_ptr<ChunkSection> parentSection = this -> getParentSecton();
//
//    if (parentSection == nullptr) {
//        return BlockId_Air;
//    }
//
//    std::shared_ptr<ChunkBlock> block = parentSection ->  getBlock(destPosition.x, destPosition.y, destPosition.z);
//    if (block == nullptr) {
//        // 如果返回nulllptr说明超出这个section的范围了，那么我们再在chunk的层面做剔除
//        // 需要找到自己上面一个section的底部对应的block是否是air
//        GLuint indexInChunk = parentSection -> getIndexOfParentChunkSections();
//        std::shared_ptr<Chunk> parentChunk = parentSection -> getParentChunk();
//        if (parentChunk == nullptr) {
//            return BlockId_Air;
//        }
//        std::shared_ptr<ChunkSection> nextSection = parentChunk -> getSection(indexInChunk + 1);
//        if (nextSection == nullptr) {
//            return BlockId_Air;
//        }
//
//        // 当前block的上面一个section的底部对应的block
//        std::shared_ptr<ChunkBlock> nextSectionBlock = nextSection -> getBlock(positionInSection.x, 0, positionInSection.z);
//        if (nextSectionBlock == nullptr) {
//            return BlockId_Air;
//        }
//        return nextSectionBlock -> getBlockData().blockId;
//    }
//    return block -> getBlockData().blockId;
//}
//
//
//BlockId ChunkBlock::getDownBlockId() {
//
//    glm::vec3 positionInSection = this -> getBlockPositionInSection();
//    glm::vec3 destPosition = glm::vec3 { positionInSection.x, positionInSection.y - 1, positionInSection.z };
//    std::shared_ptr<ChunkSection> parentSection = this -> getParentSecton();
//    if (parentSection == nullptr) {
//        return BlockId_Air;
//    }
//    std::shared_ptr<ChunkBlock> block = parentSection ->  getBlock(destPosition.x, destPosition.y, destPosition.z);
//
//    if (block == nullptr) {
//        // 如果返回nulllptr说明超出这个section的范围了，那么我们再在chunk的层面做剔除
//        // 需要找到自己下面一个section的顶对应的block是否是air
//        GLuint indexInChunk = parentSection -> getIndexOfParentChunkSections();
//        std::shared_ptr<Chunk> parentChunk = parentSection -> getParentChunk();
//        if (parentChunk == nullptr) {
//            return BlockId_Air;
//        }
//
//        std::shared_ptr<ChunkSection> nextSection = parentChunk -> getSection(indexInChunk - 1);
//        if (nextSection == nullptr) {
//            return BlockId_Air;
//        }
//
//        // 当前block的下面一个secton的顶部对应的block
//        std::shared_ptr<ChunkBlock> nextSectionBlock = nextSection -> getBlock(positionInSection.x, CHUNK_SIZE - 1, positionInSection.z);
//        if (nextSectionBlock == nullptr) {
//            return BlockId_Air;
//        }
//        return nextSectionBlock -> getBlockData().blockId;
//    }
//    return block -> getBlockData().blockId;
//}
//
//
//BlockId ChunkBlock::getLeftBlockId() {
//    glm::vec3 positionInSection = this -> getBlockPositionInSection();
//    glm::vec3 destPosition = glm::vec3 {positionInSection.x - 1, positionInSection.y, positionInSection.z};
//
//    std::shared_ptr<ChunkSection> parentSection = this -> getParentSecton();
//    if (parentSection == nullptr) {
//        return BlockId_Air;
//    }
//    std::shared_ptr<ChunkBlock> block = parentSection ->  getBlock(destPosition.x, destPosition.y, destPosition.z);
//
//    if (block == nullptr) {
//        return BlockId_Air;
//    }
//    return block -> getBlockData().blockId;
//}
//
//BlockId ChunkBlock::getRightBlockId() {
//
//    glm::vec3 positionInSection = this -> getBlockPositionInSection();
//    glm::vec3 destPosition = glm::vec3{positionInSection.x + 1, positionInSection.y, positionInSection.z};
//
//    std::shared_ptr<ChunkSection> parentSection = this -> getParentSecton();
//    if (parentSection == nullptr) {
//        return BlockId_Air;
//    }
//    std::shared_ptr<ChunkBlock> block = parentSection ->  getBlock(destPosition.x, destPosition.y, destPosition.z);
//    if (block == nullptr) {
//        return BlockId_Air;
//    }
//    return block -> getBlockData().blockId;
//}
//
//BlockId ChunkBlock::getFrontBlockId() {
//
//    glm::vec3 positionInSection = this -> getBlockPositionInSection();
//    glm::vec3 destPosition = glm::vec3{positionInSection.x, positionInSection.y, positionInSection.z + 1};
//
//    std::shared_ptr<ChunkSection> parentSection = this -> getParentSecton();
//    if (parentSection == nullptr) {
//        return BlockId_Air;
//    }
//    std::shared_ptr<ChunkBlock> block = parentSection ->  getBlock(destPosition.x, destPosition.y, destPosition.z);
//
//    if (block == nullptr) {
//        return BlockId_Air;
//    }
//    return block -> getBlockData().blockId;
//}
//
//
//BlockId ChunkBlock::getBackBlockId() {
//
//    glm::vec3 positionInSection = this -> getBlockPositionInSection();
//    glm::vec3 destPosition = glm::vec3{positionInSection.x, positionInSection.y, positionInSection.z - 1};
//
//    std::shared_ptr<ChunkSection> parentSection = this -> getParentSecton();
//    if (parentSection == nullptr) {
//        return BlockId_Air;
//    }
//    std::shared_ptr<ChunkBlock> block = parentSection ->  getBlock(destPosition.x, destPosition.y, destPosition.z);
//
//    if (block == nullptr) {
//        return BlockId_Air;
//    }
//    return block -> getBlockData().blockId;
//}
