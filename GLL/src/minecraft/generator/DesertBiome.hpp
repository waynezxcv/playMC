
#ifndef DesertBiome_hpp
#define DesertBiome_hpp

#include <stdio.h>
#include "BiomeGenerator.hpp"


namespace GLL {
    class DesertBiome : public Biome {
    public:
        DesertBiome(int seed) ;
        BlockId getPlant (Rand& rand) const override;
        BlockId getTopBlock (Rand& rand) const override;
        BlockId getUnderWaterBlock   (Rand& rand) const override;
        void makeTree(Rand& rand, Chunk& chunk, int x, int y, int z) const override;
    private:
        NoiseParameters getNoiseParameters() override;
    };
}


#endif /* DesertBiome_hpp */
