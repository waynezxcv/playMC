

#ifndef TerrainGenerator_hpp
#define TerrainGenerator_hpp

#include <stdio.h>

namespace GLL {
    class Chunk;
    
    class TerrainGenerator {
    public:
        virtual void generateTerrainFor(std::shared_ptr<Chunk> chunk) = 0;
        virtual int  getMinimumSpawnHeight() const noexcept = 0;
        virtual ~TerrainGenerator() = default;
    };
    
}

#endif /* TerrainGenerator_hpp */
