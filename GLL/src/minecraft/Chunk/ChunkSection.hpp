
#ifndef ChunkSection_hpp
#define ChunkSection_hpp

#include <stdio.h>
#include <array>
#include "ChunkBlock.hpp"
#include "Frustum.hpp"


namespace GLL {
    
    class Chunk;
    /*
     体积是 16 * 16 *16 的block组合体
     */
    using ChunkBlockArrayType = std::array<std::array<std::array<std::shared_ptr<ChunkBlock>, CHUNK_SIZE>, CHUNK_SIZE>, CHUNK_SIZE>;
    
    class ChunkSection : public std::enable_shared_from_this<ChunkSection> {
        friend Chunk;
    public:
        ChunkSection(std::weak_ptr<Chunk> parentChunk, const GLuint& index);
        ~ChunkSection();
        
        /// 遍历section中的block
        /// @param callback  回调，会依次返回block，是线程安全的
        void travesingBlocks(std::function<void(std::shared_ptr<ChunkBlock>)> callback);
        
        /// 设置某个block的类型
        /// @param blockId block类型id
        /// @param x 世界坐标x
        /// @param y 世界坐标y
        /// @param z 世界坐标z
        void setBlock(const BlockId& blockId,const int& x, const int& y, const int& z);
        
        /// 获取某个坐标点上的chunkBlock对象
        /// @param x 世界坐标x
        /// @param y 世界坐标y
        /// @param z 世界坐标z
        std::shared_ptr<ChunkBlock> getBlock(const int& x, const int& y, const int& z);

        
        /// 获取section的坐标位置
        glm::vec2 getLocation() const;
        
        /// 获取在父chunk中的下标号
        GLuint getIndexOfParentChunkSections() const;
        
        
        /// 获取父级Chunk的sharedPtr
        std::shared_ptr<Chunk> getParentChunk();
        
        /// 设置为需要更新
        void setNeedUpdate();
        
        std::weak_ptr<ChunkSection> upChunkSection;
        std::weak_ptr<ChunkSection> downChunkSection;
        
        std::weak_ptr<ChunkSection> leftChunkSection;
        std::weak_ptr<ChunkSection> rightChunkSection;
        
        std::weak_ptr<ChunkSection> frontChunkSection;
        std::weak_ptr<ChunkSection> backChunkSection;
        
        AABB& getAABB();

    private:
        AABB aabb;
        std::weak_ptr<Chunk> parentChunk;
        glm::vec2 parentChunkPosition;
        glm::vec3 positionInWorld;
        GLuint indexOfParentChunkSections;
        
        ChunkBlockArrayType blockArray;
        std::mutex blockArrayMutex;
        
        std::atomic<bool> hasLoaded;
        std::atomic<bool> isNeedUpdate {false};
        
    private:
        
        void setupBlocks();
        bool isOutOfBounds(const int& x, const int& y, const int& z);
        int getBlockIndexWithBlockPosition(const int& x, const int& y, const int& z) const;
        
        
        void updateFacesX(std::shared_ptr<ChunkBlock> block, int x, int y, int z);
        void updateFacesY(std::shared_ptr<ChunkBlock> block, int x, int y, int z);
        void updateFacesZ(std::shared_ptr<ChunkBlock> block, int x, int y, int z);
        
        std::shared_ptr<ChunkSection> getUpSection();
        std::shared_ptr<ChunkSection> getDownSection();
        
        std::shared_ptr<ChunkSection> getLeftSection();
        std::shared_ptr<ChunkSection> getRightSection();
        
        std::shared_ptr<ChunkSection> getFrontSection();
        std::shared_ptr<ChunkSection> getBackSection();

    };
}

#endif /* ChunkSection_hpp */
