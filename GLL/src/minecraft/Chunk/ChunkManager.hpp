
#ifndef ChunkManager_hpp
#define ChunkManager_hpp

#include <stdio.h>
#include <array>
#include <map>
#include <unordered_map>
#include <pthread.h>

#include "VectorXZ.hpp"
#include "NonCopyable.hpp"
#include "Chunk.hpp"
#include "WorldMapGenerator.hpp"
#include "InstanceMeshDrawable.hpp"
#include "LiquidRender.hpp"
#include "FloraRender.hpp"



namespace GLL {
    
    class World;
    class ChunkRender;
    
    class ChunkManager  {
    public:
        ChunkManager(ChunkRender* chunkRender, LiquidRender* liquidRender, FloraRender* floraRender);
        ~ChunkManager();
        
        bool loadChunksIfNeeded(int x, int z);
        void unloadChunksIfNeeded(const glm::vec3& cameraPosition);
        void loadChunk(int x, int z);
        void unloadChunk(int x, int z);
        
        WorldMapGenerator& getWorldGenerator();
        std::shared_ptr<Chunk> getChunk(int x, int z);
        std::vector<std::shared_ptr<ChunkBlock>> getBlocks();
        
    private:
        // locks & containers
        pthread_mutex_t blocksLock;
        pthread_mutex_t chunkMapLock;
        pthread_mutex_t instanceMeshLock;
        std::unordered_map<VectorXZ, std::shared_ptr<Chunk>> chunkMap;
        std::vector<std::shared_ptr<ChunkBlock>> blocks;
        
        // renders
        ChunkRender* chunkRender;
        LiquidRender* liquidRender;
        FloraRender* floraRender;
        
        // others
        WorldMapGenerator worldGenerator;
        
    private:
        bool chunkExistAt(int x, int z);
        bool chunkHasLoadedAt(int x, int z);
        std::string keyStringForMesh(InstanceMesh mesh);
    };
}


#endif /* ChunkManager_hpp */
