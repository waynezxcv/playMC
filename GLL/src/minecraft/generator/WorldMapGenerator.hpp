
#ifndef WorldMapGenerator_hpp
#define WorldMapGenerator_hpp

#include <stdio.h>
#include "ChunkDefine.hpp"
#include "Array2D.hpp"
#include "TerrainGenerator.hpp"
#include "RandomSingleton.hpp"
#include "PerlinNoise.hpp"
#include "GrasslandBiome.hpp"
#include "TemperateForestBiome.hpp"
#include "DesertBiome.hpp"
#include "OceanBiome.hpp"
#include "LightForestBiome.hpp"


namespace GLL {
    
    class Chunk;
    
    class WorldMapGenerator : public TerrainGenerator {
    public:
        
        WorldMapGenerator();
        void generateTerrainFor(Chunk& chunk) override;
        int  getMinimumSpawnHeight() const noexcept override;
        
    private:
        static void setUpNoise();
        void setBlocks(int maxHeight);
        void getHeightIn (int xMin, int zMin, int xMax, int zMax);
        void getHeightMap();
        void getBiomeMap ();
        const Biome& getBiome(int x, int z) const;
        Array2D<int, CHUNK_SIZE> m_heightMap;
        Array2D<int, CHUNK_SIZE + 1> m_biomeMap;
        Random<std::minstd_rand> m_random;
        
    private:
        static PerlinNoise m_biomeNoiseGen;
        GrasslandBiome m_grassBiome;
        TemperateForestBiome m_temperateForest;
        DesertBiome m_desertBiome;
        OceanBiome m_oceanBiome;
        LightForest m_lightForest;
        Chunk* m_pChunk = nullptr;
    };
    
    
}

#endif /* WorldMapGenerator_hpp */
