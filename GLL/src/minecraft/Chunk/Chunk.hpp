
#ifndef Chunk_hpp
#define Chunk_hpp

#include <stdio.h>
#include <array>
#include "ChunkSection.hpp"
#include "WorldMapGenerator.hpp"
#include "Array2D.hpp"
#include <mutex>


namespace GLL {
    class Camera;
    class ChunkManager;
    
    class Chunk {
        
        friend ChunkManager;
        
    public:
        Chunk(const GLfloat& x, const GLfloat& z);
        ~Chunk();
        
    public:
        
        std::shared_ptr<ChunkSection> getSection(const int index);
        void setBlock(const BlockId& blockId, const GLfloat& x,  const GLfloat& y, const GLfloat& z);
        void drawChunk(Shader& shader, Camera* camera);
        
        glm::vec2 getLocation() const;
        void setLocation(const glm::vec2& location);
        
        bool getHasLoaded();
        void load(WorldMapGenerator& generator);
        int getHeightAt(int x, int z);
        
        void lockForReading();
        void unlockForReading();
        std::map<int, std::shared_ptr<ChunkSection>> sectionMap;

    private:
        void makeMeshes();
        void bufferMeshes();
        void deleteMeshBuffers();
        void cleanUp();
        
        glm::vec2 location;
        bool hasMeshMade = false;
        bool hasMeshBuffered = false;
        bool hasLoaded = false;
        
        Array2D<int , CHUNK_SIZE> highestBlocks;
        
        std::mutex sectionMapMutex;
        std::mutex updateBlockMutex;
        std::mutex readingMutex;
        std::mutex loadedMutex;
        
    private:
        static bool isOutOfBouds(const int index);
    };
}

#endif /* Chunk_hpp */
