
#ifndef ChunkSection_hpp
#define ChunkSection_hpp

#include <stdio.h>
#include <array>
#include "ChunkBlock.hpp"
#include "Frustum.hpp"

namespace GLL {
    
    class Chunk;
    class Camera;
    /*
     体积是 16 * 16 *16 的block组合体
     */
    class ChunkSection {
        
        friend Chunk;
        
    public:
        ChunkSection(const glm::vec2& parentChunkPosition, const GLuint& index);
        ~ChunkSection();
        
    public:

        std::shared_ptr<ChunkBlock> getBlock(const int x, const int y, const int z);
        static int blockIndexFromPosistionInSection(const int x, const int y, const int z);
        void fillBlocks(const BlockId& blockId);
        GLuint getIndexInParentChunk() const;
        void setParentChunk(Chunk* chunk);
        Chunk* getParentChunk();
        std::array<std::shared_ptr<ChunkBlock>, CHUNK_VOLUME>& getBlockArray();
        glm::vec3 getPositionInWorld() const;
        void drawSection(Shader& shader, Camera* camera);

    private:
        void makeMeshes();
        void bufferMeshes();
        void deleteMeshBuffers();
        void cleanUp();
        void setupBlocks();
        
        
        void setBlock(std::shared_ptr<ChunkBlock> block);
        void updateBlock(const BlockId& blockId,const int x, const int y, const int z);
        
    private:
        
        Chunk* parentChunk;
        glm::vec2 parentChunkPosition;
        GLuint indexInParentChunk;
        std::array<std::shared_ptr<ChunkBlock>, CHUNK_VOLUME> blockArray;
        bool hasMeshMade = false;
        bool hasMeshBuffered = false;
        static bool isOutOfBounds(const int x, const int y, const int z);
        
        std::mutex blockArrayMutex;
        std::mutex traversingMutex;
        
        AABB aabb;
        glm::vec3 positionInWorld;
    };
}

#endif /* ChunkSection_hpp */
