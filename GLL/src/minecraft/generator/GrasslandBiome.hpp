

#ifndef GrasslandBiome_hpp
#define GrasslandBiome_hpp

#include <stdio.h>
#include "BiomeGenerator.hpp"


namespace GLL {

    class GrasslandBiome : public Biome {
    public:
        GrasslandBiome(int seed);
        BlockId getBeachBlock(Rand& rand) const override;
        BlockId getPlant(Rand& rand) const override;
        BlockId getTopBlock(Rand& rand) const override;
        BlockId getUnderWaterBlock(Rand& rand) const override;
        void makeTree(Rand& rand, std::shared_ptr<Chunk>, int x, int y, int z) const override;
        
    private:
        NoiseParameters getNoiseParameters() override;
    };
    
}


#endif /* GrasslandBiome_hpp */
