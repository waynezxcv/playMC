
#include "TemperateForestBiome.hpp"
#include "TreeGenerator.hpp"

using namespace GLL;

TemperateForestBiome::TemperateForestBiome(int seed) :
Biome(getNoiseParameters(), 55, 75, seed) { }

BlockId TemperateForestBiome::getTopBlock(Rand& rand) const {
    return rand.intInRange(0, 10) < 8 ? BlockId_Grass : BlockId_Dirt;
}

BlockId TemperateForestBiome::getUnderWaterBlock(Rand& rand) const {
    return rand.intInRange(0, 10) > 8 ? BlockId_Dirt: BlockId_Sand;
}

void TemperateForestBiome::makeTree(Rand& rand, Chunk& chunk, int x, int y, int z) const {
    makeOakTree(chunk, rand, x, y, z);
}

NoiseParameters TemperateForestBiome::getNoiseParameters() {
    NoiseParameters heightParams;
    heightParams.octaves = 5;
    heightParams.amplitude = 100;
    heightParams.smoothness = 195;
    heightParams.heightOffset = -30;
    heightParams.roughness = 0.52;
    return heightParams;
}

BlockId TemperateForestBiome::getPlant(Rand& rand) const {
    return BlockId_TallGrass;
}
