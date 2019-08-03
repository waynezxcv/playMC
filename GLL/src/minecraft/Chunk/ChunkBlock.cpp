

#include "ChunkBlock.hpp"
#include "ChunkSection.hpp"
#include "Chunk.hpp"


using namespace GLL;

ChunkBlock::ChunkBlock(const BlockId& blockId, const glm::vec3& blockPositionInSection, const glm::vec3& blockPositionInWorld) :
blockData(BlockDataFactory::sharedInstance() -> getBlockDataConent(blockId)) ,
blockPositionInSection(blockPositionInSection) ,
blockPositonInWorld(blockPositionInWorld)
{
}

ChunkBlock::~ChunkBlock() {
    deleteMeshBuffers();
}

void ChunkBlock::updateBlockId(const BlockId& blockId) {
    blockData = BlockDataFactory::sharedInstance() -> getBlockDataConent(blockId);
}


void ChunkBlock::setParentSection(ChunkSection* section) {
    this -> parentSection = section;
}

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
        BlockId down = this -> getDownBlock();
        if (down == BlockId_Air) {
            InstanceMesh mesh;
            mesh.blockData = blockData;
            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_NegativeY;
            mesh.offset = blockPositonInWorld;
            instanceMeshes.push_back(mesh);
        }
        // 顶部
        BlockId up = this -> getUpBlock();
        if (up == BlockId_Air) {
            
            InstanceMesh mesh;
            mesh.blockData = blockData;
            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_PositiveY;
            mesh.offset = blockPositonInWorld;
            instanceMeshes.push_back(mesh);
        }
        // 左边
        BlockId left = this -> getLeftBlock();
        if (left == BlockId_Air) {
            InstanceMesh mesh;
            mesh.blockData = blockData;
            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_NegativeX;
            mesh.offset = blockPositonInWorld;
            instanceMeshes.push_back(mesh);
        }
        // 右边
        BlockId right = this -> getRightBlock();
        if (right== BlockId_Air) {
            InstanceMesh mesh;
            mesh.blockData = blockData;
            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_PositiveX;
            mesh.offset = blockPositonInWorld;
            instanceMeshes.push_back(mesh);
        }
        // 前面
        BlockId front = this -> getFrontBlock();
        if (front == BlockId_Air) {
            InstanceMesh mesh;
            mesh.blockData = blockData;
            mesh.direction =  ChunkMesh::ChunkMeshFaceDirection_PositiveZ;
            mesh.offset = blockPositonInWorld;
            instanceMeshes.push_back(mesh);
        }
        // 后面
        BlockId back = this -> getBackBlock();
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

void ChunkBlock::makeMeshes() {
    // 空气block当成一个占位用的block，不做任何渲染
    if (blockData.blockId == BlockId_Air) {
        return;
    }
    if (this -> hasMeshMade == true) {
        return;
    }
    
    this -> addFaceMeshIfNeeded();
    
    for (auto itetor = meshes.begin(); itetor != meshes.end(); itetor ++) {
        std::shared_ptr<ChunkMesh> mesh = *itetor;
        if (mesh != nullptr) {
            mesh -> makeMesh();
        }
    }
    this -> hasMeshMade = true;
}

void ChunkBlock::addFaceMeshIfNeeded() {
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
        BlockId down = this -> getDownBlock();
        if (down == BlockId_Air) {
            meshes.push_back(std::make_shared<ChunkMesh>(blockData, ChunkMesh::ChunkMeshFaceDirection_NegativeY, blockPositonInWorld));
        }
        // 顶部
        BlockId up = this -> getUpBlock();
        if (up== BlockId_Air) {
            meshes.push_back(std::make_shared<ChunkMesh>(blockData, ChunkMesh::ChunkMeshFaceDirection_PositiveY, blockPositonInWorld));
        }
        // 左边
        BlockId left = this -> getLeftBlock();
        if (left == BlockId_Air) {
            meshes.push_back(std::make_shared<ChunkMesh>(blockData, ChunkMesh::ChunkMeshFaceDirection_NegativeX, blockPositonInWorld));
        }
        // 右边
        BlockId right = this -> getRightBlock();
        if (right== BlockId_Air) {
            meshes.push_back(std::make_shared<ChunkMesh>(blockData, ChunkMesh::ChunkMeshFaceDirection_PositiveX, blockPositonInWorld));
        }
        // 前面
        BlockId front = this -> getFrontBlock();
        if (front == BlockId_Air) {
            meshes.push_back(std::make_shared<ChunkMesh>(blockData, ChunkMesh::ChunkMeshFaceDirection_PositiveZ, blockPositonInWorld));
        }
        // 后面
        BlockId back = this -> getBackBlock();
        if (back == BlockId_Air) {
            meshes.push_back(std::make_shared<ChunkMesh>(blockData, ChunkMesh::ChunkMeshFaceDirection_NegativeZ, blockPositonInWorld));
        }
    }
    // X形不剔除了，都要显示
    else if (blockData.meshType == BlockMeshType_X) {
        meshes.push_back(std::make_shared<ChunkMesh>(blockData, ChunkMesh::ChunkMeshFaceDirection_XZ, blockPositonInWorld));
        meshes.push_back(std::make_shared<ChunkMesh>(blockData, ChunkMesh::ChunkMeshFaceDirection_ZX, blockPositonInWorld));
    }
}

void ChunkBlock::bufferMeshes() {
    // 空气block当成一个占位用的block，不做任何渲染
    if (blockData.blockId == BlockId_Air) {
        return;
    }
    if (this -> hasMeshBuffered == true) {
        return;
    }
    
    for (auto itetor = meshes.begin(); itetor != meshes.end(); itetor ++) {
        std::shared_ptr<ChunkMesh> mesh = *itetor;
        if (mesh != nullptr) {
            mesh -> bufferMesh();
        }
    }
    this -> hasMeshBuffered = true;
}


void ChunkBlock::drawBlock(Shader& shader) {
#if !INSTANCE_DRAW_ENABLED
    // 空气block当成一个占位用的block，不做任何渲染
    if (blockData.blockId == BlockId_Air) {
        return;
    }
    
    if (this -> hasMeshMade == false) {
        this -> makeMeshes();
    }
    
    if (this -> hasMeshBuffered == false) {
        this -> bufferMeshes();
    }
    TextureAtlas::sharedInstance().bindTexture();
    for (auto itetor = meshes.begin(); itetor != meshes.end(); itetor ++) {
        std::shared_ptr<ChunkMesh> mesh = *itetor;
        if (mesh == nullptr) {
            continue;
        }
#if POLYGON_LINE_ENAGLED
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
        mesh -> bindMeshVAO();
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
#if POLYGON_LINE_ENAGLED
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    }
    TextureAtlas::sharedInstance().unbindTexture();
#endif
}

void ChunkBlock::deleteMeshBuffers() {
    // 空气block当成一个占位用的block，不做任何渲染
    if (blockData.blockId == BlockId_Air) {
        return;
    }
    for (auto itetor = meshes.begin(); itetor != meshes.end(); itetor ++) {
        std::shared_ptr<ChunkMesh> mesh = *itetor;
        if (mesh != nullptr) {
            mesh -> deleteMeshBuffer();
        }
    }
    meshes.clear();
    this -> hasMeshMade = false;
    this -> hasMeshBuffered = false;
}


glm::vec3 ChunkBlock::getBlockPositonInWorld() const {
    return blockPositonInWorld;
}


glm::vec3 ChunkBlock::getBlockPositionInSection() const {
    return blockPositionInSection;
}

std::vector<std::shared_ptr<ChunkMesh>>& ChunkBlock::getChunkMeshes() {
    return this -> meshes;
}

BlockDataContent ChunkBlock::getBlockData() const {
    return blockData;
}

#pragma mark -

BlockId ChunkBlock::getUpBlock() {
#if !CUSTOM_CULL_FACE_ENABLED
    return BlockId_Air;
#endif
    glm::vec3 positionInSection = this -> getBlockPositionInSection();
    glm::vec3 destPosition = glm::vec3{positionInSection.x, positionInSection.y + 1, positionInSection.z};
    std::shared_ptr<ChunkBlock> block = this -> parentSection -> getBlock(destPosition.x, destPosition.y, destPosition.z);
    if (block == nullptr) {
        // 如果返回nulllptr说明超出这个section的范围了，那么我们再在chunk的层面做剔除
        // 需要找到自己上面一个section的底部对应的block是否是air
        ChunkSection* parentSection = this -> parentSection;
        GLuint indexInChunk = parentSection -> getIndexInParentChunk();
        Chunk* parentChunk = this -> parentSection -> getParentChunk();
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


BlockId ChunkBlock::getDownBlock() {
#if !CUSTOM_CULL_FACE_ENABLED
    return BlockId_Air;
#endif
    glm::vec3 positionInSection = this -> getBlockPositionInSection();
    glm::vec3 destPosition = glm::vec3{positionInSection.x, positionInSection.y - 1, positionInSection.z};
    std::shared_ptr<ChunkBlock> block = this -> parentSection -> getBlock(destPosition.x, destPosition.y, destPosition.z);
    if (block == nullptr) {
        // 如果返回nulllptr说明超出这个section的范围了，那么我们再在chunk的层面做剔除
        // 需要找到自己下面一个section的顶对应的block是否是air
        ChunkSection* parentSection = this -> parentSection;
        GLuint indexInChunk = parentSection -> getIndexInParentChunk();
        Chunk* parentChunk = this -> parentSection -> getParentChunk();
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

BlockId ChunkBlock::getLeftBlock() {
#if !CUSTOM_CULL_FACE_ENABLED
    return BlockId_Air;
#endif
    glm::vec3 positionInSection = this -> getBlockPositionInSection();
    glm::vec3 destPosition = glm::vec3{positionInSection.x - 1, positionInSection.y, positionInSection.z};
    std::shared_ptr<ChunkBlock> block = this -> parentSection -> getBlock(destPosition.x, destPosition.y, destPosition.z);
    if (block == nullptr) {
        return BlockId_Air;
    }
    return block -> getBlockData().blockId;
}

BlockId ChunkBlock::getRightBlock() {
#if !CUSTOM_CULL_FACE_ENABLED
    return BlockId_Air;
#endif
    glm::vec3 positionInSection = this -> getBlockPositionInSection();
    glm::vec3 destPosition = glm::vec3{positionInSection.x + 1, positionInSection.y, positionInSection.z};
    std::shared_ptr<ChunkBlock> block = this -> parentSection -> getBlock(destPosition.x, destPosition.y, destPosition.z);
    if (block == nullptr) {
        return BlockId_Air;
    }
    return block -> getBlockData().blockId;
}


BlockId ChunkBlock::getFrontBlock() {
#if !CUSTOM_CULL_FACE_ENABLED
    return BlockId_Air;
#endif
    glm::vec3 positionInSection = this -> getBlockPositionInSection();
    glm::vec3 destPosition = glm::vec3{positionInSection.x, positionInSection.y, positionInSection.z + 1};
    std::shared_ptr<ChunkBlock> block = this -> parentSection -> getBlock(destPosition.x, destPosition.y, destPosition.z);
    if (block == nullptr) {
        return BlockId_Air;
    }
    return block -> getBlockData().blockId;
}

BlockId ChunkBlock::getBackBlock() {
#if !CUSTOM_CULL_FACE_ENABLED
    return BlockId_Air;
#endif
    glm::vec3 positionInSection = this -> getBlockPositionInSection();
    glm::vec3 destPosition = glm::vec3{positionInSection.x, positionInSection.y, positionInSection.z - 1};
    std::shared_ptr<ChunkBlock> block = this -> parentSection -> getBlock(destPosition.x, destPosition.y, destPosition.z);
    if (block == nullptr) {
        return BlockId_Air;
    }
    return block -> getBlockData().blockId;
}

std::vector<InstanceMesh>& ChunkBlock::getInstanceMeshes() {
    if (hasMadeInstanceMeshes == false) {
        this -> makeInstanceMeshes();
    }
    return instanceMeshes;
}
