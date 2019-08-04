
#ifndef ChunkBlock_hpp
#define ChunkBlock_hpp

#include <stdio.h>
#include "ChunkMesh.hpp"


namespace GLL {
    
    class ChunkSection;
    
    typedef struct {
        BlockDataContent blockData;
        ChunkMesh::ChunkMeshFaceDirection direction;
        glm::vec3 offset;
    } InstanceMesh;
    
    
    class ChunkBlock {
        friend ChunkSection;
        
    public:
        ChunkBlock(const BlockId& blockId = BlockId_Air,
                   const glm::vec3& blockPositionInSection = glm::vec3(0.0f),
                   const glm::vec3& blockPositionInWorld = glm::vec3(0.0f));
        ~ChunkBlock();
        
        void updateBlockId(const BlockId& blockId);
        void drawBlock(Shader& shader);
        
        void setParentSection(ChunkSection* section);
        glm::vec3 getBlockPositonInWorld() const;
        glm::vec3 getBlockPositionInSection() const;
        BlockDataContent getBlockData() const;
        std::vector<std::shared_ptr<ChunkMesh>>& getChunkMeshes();
        std::vector<InstanceMesh>& getInstanceMeshes();
        void makeInstanceMeshes();

    private:
        
        void makeMeshes();
        void bufferMeshes();
        void deleteMeshBuffers();
        
        BlockDataContent blockData;
        ChunkSection* parentSection;
        glm::vec3 blockPositionInSection;
        glm::vec3 blockPositonInWorld;
        
        std::vector<std::shared_ptr<ChunkMesh>> meshes;
        std::vector<InstanceMesh> instanceMeshes;

        bool hasMeshMade = false;
        bool hasMeshBuffered = false;
        bool hasMadeInstanceMeshes = false;
        
        
        BlockId getUpBlock();
        BlockId getDownBlock();
        BlockId getLeftBlock();
        BlockId getRightBlock();
        BlockId getFrontBlock();
        BlockId getBackBlock();
        void addFaceMeshIfNeeded();

    };
}

#endif /* ChunkBlock_hpp */
