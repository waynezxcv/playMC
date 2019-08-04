
#include "DesertBiome.hpp"
#include "TreeGenerator.hpp"



using namespace GLL;

DesertBiome::DesertBiome(int seed) :
Biome  (getNoiseParameters(), 1350, 500, seed) {
    
}

BlockId DesertBiome::getTopBlock(Rand& rand) const {
    return BlockId_Sand;
}

BlockId DesertBiome::getUnderWaterBlock(Rand& rand) const {
    return BlockId_Sand;
}

void DesertBiome::makeTree(Rand& rand, Chunk& chunk, int x, int y, int z) const {
    
    if (y < WATER_LEVEL + 15) {
        if (rand.intInRange(0, 100) > 75) {
            makePalmTree(chunk, rand, x, y, z);
        }
        else {
            makeCactus(chunk, rand, x, y, z);
        }
    }
    else {
        makeCactus(chunk, rand, x, y, z);
    }
}

NoiseParameters DesertBiome::getNoiseParameters() {
    NoiseParameters heightParams;
    heightParams.octaves = 9;
    heightParams.amplitude = 80;
    heightParams.smoothness = 335;
    heightParams.heightOffset = -7;
    heightParams.roughness = 0.56;
    return heightParams;
}

BlockId DesertBiome::getPlant(Rand& rand) const {
    return BlockId_DeadShrub;
}
