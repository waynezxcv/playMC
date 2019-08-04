
#ifndef Chunk_hpp
#define Chunk_hpp


#include <stdio.h>
#include <mutex>
#include <memory>
#include <unordered_map>
#include "ChunkSection.hpp"
#include "WorldMapGenerator.hpp"

namespace GLL {
    
    class ChunkManager;
    
    /// Chunk是一个由16个Section组成的结构，他包含16 * 16 * 16 *16个block
    class Chunk : public std::enable_shared_from_this<Chunk> {
        
        friend ChunkManager;
        
    public:
        Chunk(const GLfloat& x, const GLfloat& z);
        ~Chunk();
        
    public:
        /// 遍历chunk中的section
        /// @param callback  回调，会依次返回section，是线程安全的
        void traversingSections(std::function<void(std::shared_ptr<ChunkSection>)> callback);
        
        /// 设置某个block的类型
        /// @param blockId block类型id
        /// @param x 世界坐标x
        /// @param y 世界坐标y
        /// @param z 世界坐标z
        void setBlock(const BlockId& blockId, const GLfloat& x,  const GLfloat& y, const GLfloat& z);
        
        /// 通过下标来获取一个section
        /// @param index section在chunk中的下标
        std::shared_ptr<ChunkSection> getSection(const int index);
        
        
        /// 获取当前chunk的坐标值，这是一个只有x，z两个值的坐标，对应chunk的左下角的坐标
        glm::vec2 getLocation() const;
        
        /// 获取当前chunk是否已经加载过了
        bool getHasLoaded();
    private:
        glm::vec2 location;
        std::atomic<bool> hasLoaded;
        std::mutex sectionMapMutex;
        std::unordered_map<int, std::shared_ptr<ChunkSection>> sectionMap;
    private:
        void setupSections();
        bool isOutOfBouds(const int index);
        void load(WorldMapGenerator& generator);
    };
}

#endif /* Chunk_hpp */
