
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
        ChunkBlock(std::weak_ptr<ChunkSection> parentSection,
        const BlockId& blockId = BlockId_Air,
        const glm::vec3& blockPositionInSection = glm::vec3(0.0f),
        const glm::vec3& blockPositionInWorld = glm::vec3(0.0f));
        
        glm::vec3 getBlockPositonInWorld() const;
        glm::vec3 getBlockPositionInSection() const;
        BlockDataContent getBlockData() const;
        void updateBlockId(const BlockId& blockId);
        
        std::vector<InstanceMesh>& getInstanceMeshes();
        std::shared_ptr<ChunkSection> getParentSecton();
    private:
        
        BlockDataContent blockData;
        glm::vec3 blockPositionInSection;
        glm::vec3 blockPositonInWorld;
        std::weak_ptr<ChunkSection> parentSection;
        std::vector<InstanceMesh> instanceMeshes;
        
    private:
        
        std::atomic<bool> hasMadeInstanceMeshes;
        
        void makeInstanceMeshes();
        BlockId getUpBlockId();
        BlockId getDownBlockId();
        BlockId getLeftBlockId();
        BlockId getRightBlockId();
        BlockId getFrontBlockId();
        BlockId getBackBlockId();
    };
}

#endif /* ChunkBlock_hpp */
