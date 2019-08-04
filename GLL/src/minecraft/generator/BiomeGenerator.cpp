
#include "BiomeGenerator.hpp"

using namespace GLL;

Biome::Biome(const NoiseParameters& parameters, int treeFreq, int plantFreq, int seed) :
heightGenerator (seed),
treeFreq (treeFreq),
plantFreq (plantFreq) {
    heightGenerator.setParameters(parameters);
}

BlockId Biome::getBeachBlock(Random<std::minstd_rand>& rand) const {
    return BlockId_Sand;
}

int Biome::getHeight(int x, int z, int chunkX, int chunkZ) const {
    return heightGenerator.getHeight(x, z, chunkX, chunkZ);
}

int Biome::getTreeFrequency() const noexcept {
    return treeFreq;
}

int Biome::getPlantFrequency () const noexcept {
    return plantFreq;
}

