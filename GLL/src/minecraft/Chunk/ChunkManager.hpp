
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


namespace GLL {
    
    class World;
    class ChunkRender;
    
    class ChunkManager  {
    public:
        ChunkManager(ChunkRender* chunkRender, ChunkRender* liquidRender, ChunkRender* floraRender);
        ~ChunkManager();
        void loadChunksIfNeeded(int x, int z);
        void unloadChunksIfNeeded(const glm::vec3& cameraPosition);
        bool loadChunk(int x, int z);
        void unloadChunk(int x, int z);
        WorldMapGenerator& getWorldGenerator();
        std::shared_ptr<Chunk> getChunk(int x, int z);
        std::vector<std::shared_ptr<ChunkBlock>> getBlocks();
    private:
        // locks & containers
        pthread_mutex_t blocksLock;
        pthread_mutex_t chunkMapLock;
        pthread_mutex_t instanceLock;
        
        std::unordered_map<VectorXZ, std::shared_ptr<Chunk>> chunkMap;
        std::vector<std::shared_ptr<ChunkBlock>> blocks;
        std::map<std::string, std::shared_ptr<InstanceMeshDrawable>> instanceMeshDrawables;
        
        // renders
        ChunkRender* chunkRender;
        ChunkRender* liquidRender;
        ChunkRender* floraRender;
        // others
        WorldMapGenerator worldGenerator;
        
    private:
        bool chunkExistAt(int x, int z);
        bool chunkHasLoadedAt(int x, int z);
        std::string keyStringForMesh(InstanceMesh mesh);
        void intenalLoadChunk(int x, int z);
    };
}


#endif /* ChunkManager_hpp */
