
#ifndef Chunk_hpp
#define Chunk_hpp


#include <stdio.h>
#include <mutex>
#include <memory>
#include <set>
#include <unordered_map>
#include "ChunkSection.hpp"
#include "WorldMapGenerator.hpp"
#include "VectorXZ.hpp"


namespace GLL {
    class ChunkManager;
    class InstanceMeshDrawable;
    class MasterRender;
}


namespace GLL {
    
    /// Chunk是一个由16个Section组成的结构，他包含16 * 16 * 16 *16个block
    class Chunk : public std::enable_shared_from_this<Chunk> {
        friend ChunkManager;
        
    public:
        
        static VectorXZ normalizeChunkCoordination(int x, int z);
        
        
    public:
        Chunk(const GLfloat& x, const GLfloat& z);
        ~Chunk();
        bool makeMeshIfNeeded(MasterRender& render);
        bool unMakeMeshIfNeeded(MasterRender& render);

    public:
        /// 遍历chunk中的section
        /// @param callback  回调，会依次返回section，是线程安全的
        void traversingSections(std::function<void(std::shared_ptr<ChunkSection>)> callback);
        
        /// 设置某个block的类型
        /// @param blockId block类型id
        /// @param x 世界坐标x
        /// @param y 世界坐标y
        /// @param z 世界坐标z
        void setBlock(const BlockId& blockId, GLfloat x,  GLfloat y, GLfloat z);
        
        /// 通过下标来获取一个section
        /// @param index section在chunk中的下标
        std::shared_ptr<ChunkSection> getSection(const int index);
        
        /// 获取当前chunk的坐标值，这是一个只有x，z两个值的坐标，对应chunk的左下角的坐标
        glm::vec2 getLocation() const;
        
        /// 获取当前chunk是否已经加载过了
        bool getHasLoaded();
        
        /// 获取高度
        int getHeightAt(int x, int z) const;
        
        /// 根据世界坐标获取到某个block
        std::shared_ptr<ChunkBlock> getBlock(int x, int y, int z);
        std::weak_ptr<Chunk> leftChunk;
        std::weak_ptr<Chunk> rightChunk;
        std::weak_ptr<Chunk> frontChunk;
        std::weak_ptr<Chunk> backChunk;
        
    private:
        std::atomic<bool> hasLoaded {false};
        glm::vec2 location;
        std::mutex sectionMapMutex;
        std::unordered_map<int, std::shared_ptr<ChunkSection>> sectionMap;
        
        mutable std::mutex highesetBlocksMutex;
        std::unordered_map<VectorXZ, int> highestBlocks;
        
    private:
        
        bool isOutOfBouds(const int index);
        void load(WorldMapGenerator& generator);
        void setupSectionIfNeeded(int index);
        void updateHighestBlocks(int x, int y, int z);
        std::vector<std::shared_ptr<InstanceMesh>> makeMeshesWithBlock(std::shared_ptr<ChunkBlock>block);
        std::string hashWithInstanceMesh(std::shared_ptr<InstanceMesh> mesh);
        
        void updateNearestChunkSections(std::shared_ptr<ChunkSection> section);
        std::shared_ptr<Chunk> getLeftChunk();
        std::shared_ptr<Chunk> getRightChunk();
        std::shared_ptr<Chunk> getFrontChunk();
        std::shared_ptr<Chunk> getBackChunk();
    };
}

#endif /* Chunk_hpp */
