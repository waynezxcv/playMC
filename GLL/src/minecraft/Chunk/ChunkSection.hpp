
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
        
    private:
        
        void setupBlocks();
        std::weak_ptr<Chunk> parentChunk;
        glm::vec2 parentChunkPosition;
        glm::vec3 positionInWorld;
        GLuint indexOfParentChunkSections;
        AABB aabb;
        std::array<std::shared_ptr<ChunkBlock>, CHUNK_VOLUME> blockArray;
        std::mutex blockArrayMutex;
        std::atomic<bool> hasLoaded;
        
    private:
        bool isOutOfBounds(const int& x, const int& y, const int& z);
        int getBlockIndexWithBlockPosition(const int& x, const int& y, const int& z) const;
    };
}


#endif /* ChunkSection_hpp */
