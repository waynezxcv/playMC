
#ifndef ChunkBlock_hpp
#define ChunkBlock_hpp

#include <stdio.h>
#include "ChunkMesh.hpp"


namespace GLL {
    
    class InstanceMesh {
    public:
        InstanceMesh() {};
        ~InstanceMesh() {};
        BlockDataContent blockData;
        ChunkMesh::ChunkMeshFaceDirection direction;
        glm::vec3 offset;
    };
}

namespace GLL {
    class ChunkSection;
}


namespace GLL {
    class ChunkBlock {
        friend ChunkSection;
    public:
        ChunkBlock(std::weak_ptr<ChunkSection> parentSection, const BlockId& blockId = BlockId_Air, const glm::vec3& blockPositionInSection = glm::vec3(0.0f), const glm::vec3& blockPositionInWorld = glm::vec3(0.0f));
        ~ChunkBlock();
    public:
        glm::vec3 getBlockPositonInWorld() const;
        glm::vec3 getBlockPositionInSection() const;
        BlockDataContent getBlockData() const;
        void updateBlockId(const BlockId& blockId);
        std::shared_ptr<ChunkSection> getParentSecton();
        
        BlockId upBlockId = BlockId_Air;
        BlockId downBlockId = BlockId_Air;
        BlockId frontBlockId = BlockId_Air;
        BlockId backBlockId = BlockId_Air;
        BlockId leftBlockId = BlockId_Air;
        BlockId rightBlockId = BlockId_Air;
        
    private:
        BlockDataContent blockData;
        glm::vec3 blockPositionInSection;
        glm::vec3 blockPositonInWorld;
        std::weak_ptr<ChunkSection> parentSection;
    };
}


#endif /* ChunkBlock_hpp */
