
#include "OceanBiome.hpp"
#include "TreeGenerator.hpp"

using namespace GLL;



OceanBiome::OceanBiome(int seed) :
Biome(getNoiseParameters(), 50, 100, seed) {}

BlockId OceanBiome::getTopBlock(Rand& rand) const {
    return BlockId_Grass;
}


BlockId OceanBiome::getUnderWaterBlock(Rand& rand) const {
    return BlockId_Sand;
}


void OceanBiome::makeTree(Rand& rand, Chunk& chunk, int x, int y, int z) const {
    rand.intInRange(0, 5) < 3 ?
    makePalmTree(chunk, rand, x, y, z) :
    makeOakTree (chunk, rand, x, y, z);
}


NoiseParameters OceanBiome::getNoiseParameters() {
    NoiseParameters heightParams;
    heightParams.octaves = 7;
    heightParams.amplitude = 43;
    heightParams.smoothness = 55;
    heightParams.heightOffset = 0;
    heightParams.roughness = 0.50;
    return heightParams;
}


BlockId OceanBiome::getPlant(Rand& rand) const {
    return rand.intInRange(0, 10) > 6 ? BlockId_Rose : BlockId_TallGrass;
}

