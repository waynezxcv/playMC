

#ifndef BiomeGenerator_hpp
#define BiomeGenerator_hpp

#include <stdio.h>
#include "PerlinNoise.hpp"
#include "RandomSingleton.hpp"
#include "ChunkBlock.hpp"


namespace GLL {
    
    using Rand = Random<std::minstd_rand>;
    class Chunk;
    
    
    class Biome {
    public:
        
        Biome(const NoiseParameters& parameters, int treeFreq, int plantFreq, int seed);
        virtual ~Biome() = default;
        virtual ChunkBlock getPlant (Random<std::minstd_rand>& rand) const = 0;
        virtual ChunkBlock getTopBlock (Random<std::minstd_rand>& rand) const = 0;
        virtual ChunkBlock getUnderWaterBlock (Random<std::minstd_rand>& rand) const = 0;
        virtual ChunkBlock getBeachBlock (Random<std::minstd_rand>& rand) const;
        virtual void makeTree (Random<std::minstd_rand>& rand, Chunk& chunk, int x, int y, int z) const = 0;
        
        int getHeight(int x, int z, int chunkX, int chunkZ) const;
        int getTreeFrequency () const noexcept;
        int getPlantFrequency () const noexcept;
        
    protected:
        virtual NoiseParameters getNoiseParameters() = 0;
        
    private:
        PerlinNoise m_heightGenerator;
        int m_treeFreq;
        int m_plantFreq;
    };
}

#endif /* BiomeGenerator_hpp */
