

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
        virtual BlockId getPlant (Random<std::minstd_rand>& rand) const = 0;
        virtual BlockId getTopBlock (Random<std::minstd_rand>& rand) const = 0;
        virtual BlockId getUnderWaterBlock (Random<std::minstd_rand>& rand) const = 0;
        virtual BlockId getBeachBlock (Random<std::minstd_rand>& rand) const;
        virtual void makeTree (Random<std::minstd_rand>& rand, Chunk& chunk, int x, int y, int z) const = 0;
        
        int getHeight(int x, int z, int chunkX, int chunkZ) const;
        int getTreeFrequency () const noexcept;
        int getPlantFrequency () const noexcept;
        
    protected:
        virtual NoiseParameters getNoiseParameters() = 0;
        
    private:
        PerlinNoise heightGenerator;
        int treeFreq;
        int plantFreq;
    };
}

#endif /* BiomeGenerator_hpp */
