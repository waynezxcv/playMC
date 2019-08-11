

#ifndef LightForestBiome_hpp
#define LightForestBiome_hpp

#include <stdio.h>
#include "BiomeGenerator.hpp"


namespace GLL {
    
    class LightForest : public Biome {
    public:
        LightForest(int seed);
        BlockId getPlant(Rand& rand) const override;
        BlockId getTopBlock(Rand& rand) const override;
        BlockId getUnderWaterBlock(Rand& rand) const override;
        void makeTree(Rand& rand, std::shared_ptr<Chunk>, int x, int y, int z) const override;
    private:
        NoiseParameters getNoiseParameters() override;
    };
    
}



#endif /* LightForestBiome_hpp */
