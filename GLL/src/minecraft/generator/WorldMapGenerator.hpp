
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
        static PerlinNoise biomeNoiseGen;
        GrasslandBiome grassBiome;
        TemperateForestBiome temperateForest;
        DesertBiome desertBiome;
        OceanBiome oceanBiome;
        LightForest lightForest;
        Chunk* chunk = nullptr;
    };
    
    
}

#endif /* WorldMapGenerator_hpp */
