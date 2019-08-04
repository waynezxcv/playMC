
#include "GrasslandBiome.hpp"
#include "TreeGenerator.hpp"

using namespace GLL;


GrasslandBiome::GrasslandBiome(int seed) :
Biome  (getNoiseParameters(), 1000, 20, seed) {
}

BlockId GrasslandBiome::getTopBlock(Rand& rand) const {
    return BlockId_Grass;
}

BlockId GrasslandBiome::getUnderWaterBlock(Rand& rand) const {
    return rand.intInRange(0, 10) > 8 ? BlockId_Dirt : BlockId_Sand;
}


BlockId GrasslandBiome::getBeachBlock (Rand& rand) const {
    return rand.intInRange(0, 10) > 2 ? BlockId_Grass: BlockId_Dirt;
}



void GrasslandBiome::makeTree(Rand& rand, Chunk& chunk, int x, int y, int z) const {
    makeOakTree(chunk, rand, x, y, z);
}

NoiseParameters GrasslandBiome::getNoiseParameters() {
    NoiseParameters heightParams;
    heightParams.octaves = 9;
    heightParams.amplitude = 85;
    heightParams.smoothness = 235;
    heightParams.heightOffset = -20;
    heightParams.roughness = 0.51;
    return heightParams;
}

BlockId GrasslandBiome::getPlant(Rand& rand) const {
    return rand.intInRange(0, 10) > 6 ? BlockId_Rose : BlockId_TallGrass;
}

