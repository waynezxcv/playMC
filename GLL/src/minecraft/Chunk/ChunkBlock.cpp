

#include "ChunkBlock.hpp"
#include "ChunkSection.hpp"
#include "Chunk.hpp"

using namespace GLL;

#pragma mark - LifeCycle

ChunkBlock::ChunkBlock(std::weak_ptr<ChunkSection> parentSection,
const BlockId& blockId,
const glm::vec3& blockPositionInSection ,
const glm::vec3& blockPositionInWorld) :

parentSection(parentSection),
blockData(BlockDataFactory::sharedInstance() -> getBlockDataConent(blockId)) ,
blockPositionInSection(blockPositionInSection) ,
blockPositonInWorld(blockPositionInWorld)
{
    
}

#pragma mark - Public Interface

void ChunkBlock::updateBlockId(const BlockId& blockId) {
    blockData = BlockDataFactory::sharedInstance() -> getBlockDataConent(blockId);
}


#pragma mark - Private Interface

void ChunkBlock::makeInstanceMeshes() {
    
    if (hasMadeInstanceMeshes == true) {
        return;
    }
    hasMadeInstanceMeshes = true;
    
    // 空气block当成一个占位用的block，不做任何渲染
    if (blockData.blockId == BlockId_Air) {
        return;
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
        BlockId down = this -> getDownBlockId();
        if (down == BlockId_Air) {
            InstanceMesh mesh;
            mesh.blockData = blockData;
            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_NegativeY;
            mesh.offset = blockPositonInWorld;
            instanceMeshes.push_back(mesh);
        }
        
        // 顶部
        BlockId up = this -> getUpBlockId();
        if (up == BlockId_Air) {
            
            InstanceMesh mesh;
            mesh.blockData = blockData;
            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_PositiveY;
            mesh.offset = blockPositonInWorld;
            instanceMeshes.push_back(mesh);
        }
        
        // 左边
        BlockId left = this -> getLeftBlockId();
        if (left == BlockId_Air) {
            InstanceMesh mesh;
            mesh.blockData = blockData;
            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_NegativeX;
            mesh.offset = blockPositonInWorld;
            instanceMeshes.push_back(mesh);
        }
        
        // 右边
        BlockId right = this -> getRightBlockId();
        if (right== BlockId_Air) {
            InstanceMesh mesh;
            mesh.blockData = blockData;
            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_PositiveX;
            mesh.offset = blockPositonInWorld;
            instanceMeshes.push_back(mesh);
        }
        
        // 前面
        BlockId front = this -> getFrontBlockId();
        if (front == BlockId_Air) {
            InstanceMesh mesh;
            mesh.blockData = blockData;
            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_PositiveZ;
            mesh.offset = blockPositonInWorld;
            instanceMeshes.push_back(mesh);
        }
        
        // 后面
        BlockId back = this -> getBackBlockId();
        if (back == BlockId_Air ) {
            InstanceMesh mesh;
            mesh.blockData = blockData;
            mesh.direction = ChunkMesh::ChunkMeshFaceDirection_NegativeZ;
            mesh.offset = blockPositonInWorld;
            instanceMeshes.push_back(mesh);
        }
    }
    
    
    // X形不剔除了，都要显示
    else if (blockData.meshType == BlockMeshType_X) {
        
        InstanceMesh mesh1;
        mesh1.blockData = blockData;
        mesh1.direction =  ChunkMesh::ChunkMeshFaceDirection_XZ;
        mesh1.offset = blockPositonInWorld;
        instanceMeshes.push_back(mesh1);
        
        InstanceMesh mesh2;
        mesh2.blockData = blockData;
        mesh2.direction =  ChunkMesh::ChunkMeshFaceDirection_ZX;
        mesh2.offset = blockPositonInWorld;
        instanceMeshes.push_back(mesh2);
        
    }
}


BlockId ChunkBlock::getUpBlockId() {
    
    glm::vec3 positionInSection = this -> getBlockPositionInSection();
    glm::vec3 destPosition = glm::vec3{positionInSection.x, positionInSection.y + 1, positionInSection.z};
    std::shared_ptr<ChunkSection> parentSection = this -> getParentSecton();
    
    if (parentSection == nullptr) {
        return BlockId_Air;
    }
    
    std::shared_ptr<ChunkBlock> block = parentSection ->  getBlock(destPosition.x, destPosition.y, destPosition.z);
    if (block == nullptr) {
        // 如果返回nulllptr说明超出这个section的范围了，那么我们再在chunk的层面做剔除
        // 需要找到自己上面一个section的底部对应的block是否是air
        GLuint indexInChunk = parentSection -> getIndexOfParentChunkSections();
        std::shared_ptr<Chunk> parentChunk = parentSection -> getParentChunk();
        if (parentChunk == nullptr) {
            return BlockId_Air;
        }
        std::shared_ptr<ChunkSection> nextSection = parentChunk -> getSection(indexInChunk + 1);
        if (nextSection == nullptr) {
            return BlockId_Air;
        }
        
        // 当前block的上面一个section的底部对应的block
        std::shared_ptr<ChunkBlock> nextSectionBlock = nextSection -> getBlock(positionInSection.x, 0, positionInSection.z);
        if (nextSectionBlock == nullptr) {
            return BlockId_Air;
        }
        return nextSectionBlock -> getBlockData().blockId;
    }
    return block -> getBlockData().blockId;
}


BlockId ChunkBlock::getDownBlockId() {
    
    glm::vec3 positionInSection = this -> getBlockPositionInSection();
    glm::vec3 destPosition = glm::vec3 { positionInSection.x, positionInSection.y - 1, positionInSection.z };
    std::shared_ptr<ChunkSection> parentSection = this -> getParentSecton();
    if (parentSection == nullptr) {
        return BlockId_Air;
    }
    std::shared_ptr<ChunkBlock> block = parentSection ->  getBlock(destPosition.x, destPosition.y, destPosition.z);
    
    if (block == nullptr) {
        // 如果返回nulllptr说明超出这个section的范围了，那么我们再在chunk的层面做剔除
        // 需要找到自己下面一个section的顶对应的block是否是air
        GLuint indexInChunk = parentSection -> getIndexOfParentChunkSections();
        std::shared_ptr<Chunk> parentChunk = parentSection -> getParentChunk();
        if (parentChunk == nullptr) {
            return BlockId_Air;
        }
        
        std::shared_ptr<ChunkSection> nextSection = parentChunk -> getSection(indexInChunk - 1);
        if (nextSection == nullptr) {
            return BlockId_Air;
        }
        
        // 当前block的下面一个secton的顶部对应的block
        std::shared_ptr<ChunkBlock> nextSectionBlock = nextSection -> getBlock(positionInSection.x, CHUNK_SIZE - 1, positionInSection.z);
        if (nextSectionBlock == nullptr) {
            return BlockId_Air;
        }
        return nextSectionBlock -> getBlockData().blockId;
    }
    return block -> getBlockData().blockId;
}


BlockId ChunkBlock::getLeftBlockId() {
    glm::vec3 positionInSection = this -> getBlockPositionInSection();
    glm::vec3 destPosition = glm::vec3 {positionInSection.x - 1, positionInSection.y, positionInSection.z};
    
    std::shared_ptr<ChunkSection> parentSection = this -> getParentSecton();
    if (parentSection == nullptr) {
        return BlockId_Air;
    }
    std::shared_ptr<ChunkBlock> block = parentSection ->  getBlock(destPosition.x, destPosition.y, destPosition.z);
    
    if (block == nullptr) {
        return BlockId_Air;
    }
    return block -> getBlockData().blockId;
}

BlockId ChunkBlock::getRightBlockId() {
    
    glm::vec3 positionInSection = this -> getBlockPositionInSection();
    glm::vec3 destPosition = glm::vec3{positionInSection.x + 1, positionInSection.y, positionInSection.z};
    
    std::shared_ptr<ChunkSection> parentSection = this -> getParentSecton();
    if (parentSection == nullptr) {
        return BlockId_Air;
    }
    std::shared_ptr<ChunkBlock> block = parentSection ->  getBlock(destPosition.x, destPosition.y, destPosition.z);
    if (block == nullptr) {
        return BlockId_Air;
    }
    return block -> getBlockData().blockId;
}

BlockId ChunkBlock::getFrontBlockId() {
    
    glm::vec3 positionInSection = this -> getBlockPositionInSection();
    glm::vec3 destPosition = glm::vec3{positionInSection.x, positionInSection.y, positionInSection.z + 1};
    
    std::shared_ptr<ChunkSection> parentSection = this -> getParentSecton();
    if (parentSection == nullptr) {
        return BlockId_Air;
    }
    std::shared_ptr<ChunkBlock> block = parentSection ->  getBlock(destPosition.x, destPosition.y, destPosition.z);
    
    if (block == nullptr) {
        return BlockId_Air;
    }
    return block -> getBlockData().blockId;
}


BlockId ChunkBlock::getBackBlockId() {
    
    glm::vec3 positionInSection = this -> getBlockPositionInSection();
    glm::vec3 destPosition = glm::vec3{positionInSection.x, positionInSection.y, positionInSection.z - 1};
    
    std::shared_ptr<ChunkSection> parentSection = this -> getParentSecton();
    if (parentSection == nullptr) {
        return BlockId_Air;
    }
    std::shared_ptr<ChunkBlock> block = parentSection ->  getBlock(destPosition.x, destPosition.y, destPosition.z);
    
    if (block == nullptr) {
        return BlockId_Air;
    }
    return block -> getBlockData().blockId;
}

#pragma mark - Getter

glm::vec3 ChunkBlock::getBlockPositonInWorld() const {
    return blockPositonInWorld;
}

glm::vec3 ChunkBlock::getBlockPositionInSection() const {
    return blockPositionInSection;
}

BlockDataContent ChunkBlock::getBlockData() const {
    return blockData;
}

std::vector<InstanceMesh>& ChunkBlock::getInstanceMeshes() {
    if (hasMadeInstanceMeshes == false) {
        this -> makeInstanceMeshes();
    }
    return instanceMeshes;
}

std::shared_ptr<ChunkSection> ChunkBlock::getParentSecton() {
    if (std::shared_ptr<ChunkSection> sp  = this -> parentSection.lock()) {
        return sp;
    }
    else {
        return nullptr;
    }
}
