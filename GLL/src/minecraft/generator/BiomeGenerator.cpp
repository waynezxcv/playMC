
#include "BiomeGenerator.hpp"

using namespace GLL;

Biome::Biome(const NoiseParameters& parameters, int treeFreq, int plantFreq, int seed) :
m_heightGenerator (seed),
m_treeFreq (treeFreq),
m_plantFreq (plantFreq) {
    m_heightGenerator.setParameters(parameters);
}

ChunkBlock Biome::getBeachBlock(Random<std::minstd_rand>& rand) const {
    return BlockId_Sand;
}

int Biome::getHeight(int x, int z, int chunkX, int chunkZ) const {
    return m_heightGenerator.getHeight(x, z, chunkX, chunkZ);
}

int Biome::getTreeFrequency() const noexcept {
    return m_treeFreq;
}

int Biome::getPlantFrequency () const noexcept {
    return m_plantFreq;
}

