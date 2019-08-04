
#ifndef TemperateForestBiome_hpp
#define TemperateForestBiome_hpp

#include <stdio.h>
#include "BiomeGenerator.hpp"


namespace GLL {
    class TemperateForestBiome : public Biome {
    public:
        TemperateForestBiome(int seed);
        BlockId getPlant(Rand& rand) const override;
        BlockId getTopBlock(Rand& rand) const override;
        BlockId getUnderWaterBlock(Rand& rand) const override;
        void makeTree(Rand& rand, Chunk& chunk, int x, int y, int z) const override;
    private:
        NoiseParameters getNoiseParameters() override;
    };
}


#endif /* TemperateForestBiome_hpp */
