
#include "WorldMapGenerator.hpp"
#include <functional>
#include <algorithm>
#include <random>
#include "Chunk.hpp"
#include "TreeGenerator.hpp"
#include "GeneralMaths.hpp"


using namespace GLL;

namespace {
    const int seed = RandomSingleton::sharedInstance().intInRange(424, 325322);
}

PerlinNoise WorldMapGenerator::biomeNoiseGen(seed * 2);



WorldMapGenerator::WorldMapGenerator() :
grassBiome (seed),
temperateForest (seed),
desertBiome (seed),
oceanBiome (seed),
lightForest (seed),
grasslandBiome(seed)
{
    setUpNoise();
}


void WorldMapGenerator::setUpNoise() {
    std::cout << "Seed: " << seed << '\n';
    static bool noiseGen = false;
    if (!noiseGen) {
        std::cout << "start making noise ... \n";
        noiseGen = true;
        NoiseParameters biomeParmams;
        biomeParmams.octaves = 5;
        biomeParmams.amplitude = 120;
        biomeParmams.smoothness = 1035;
        biomeParmams.heightOffset = 0;
        biomeParmams.roughness = 0.75;
        biomeNoiseGen.setParameters(biomeParmams);
    }
}


void WorldMapGenerator::generateTerrainFor(std::shared_ptr<Chunk> chunk) {
    this -> chunk = chunk;
    auto location = chunk -> getLocation();
    
    random.setSeed(((int)location.x ^ (int)location.y) << 2);
    getBiomeMap();
    getHeightMap();
    
    auto maxHeight = heightMap.getMaxValue();
    maxHeight = maxHeight <= WATER_LEVEL ? WATER_LEVEL : maxHeight;
    setBlocks(maxHeight);
}

int WorldMapGenerator::getMinimumSpawnHeight() const noexcept {
    return WATER_LEVEL;
}


void WorldMapGenerator::getHeightIn (int xMin, int zMin, int xMax, int zMax) {
    
    auto getHeightAt = [&](int x, int z) {
        const Biome& biome = getBiome(x, z);
        return biome.getHeight(x, z, chunk->getLocation().x, chunk->getLocation().y);
    };
    
    int bottomLeft = getHeightAt(xMin, zMin);
    int bottomRight = getHeightAt(xMax, zMin);
    int topLeft = getHeightAt(xMin, zMax);
    int topRight = getHeightAt(xMax, zMax);
    
    for (int x = xMin; x < xMax; ++x ) {
        for (int z = zMin; z < zMax; ++z) {
            if (x == CHUNK_SIZE) {
                continue;
            }
            if (z == CHUNK_SIZE) {
                continue;
            }
            float h = smoothInterpolation(bottomLeft, topLeft, bottomRight, topRight, xMin, xMax, zMin, zMax, x, z);
            heightMap.get(x, z) = h;
        }
    }
}


void WorldMapGenerator::getHeightMap() {
    
    constexpr static auto HALF_CHUNK = CHUNK_SIZE / 2;
    constexpr static auto CHUNK = CHUNK_SIZE;
    
    getHeightIn(0, 0, HALF_CHUNK, HALF_CHUNK);
    getHeightIn(HALF_CHUNK, 0, CHUNK, HALF_CHUNK);
    getHeightIn(0, HALF_CHUNK, HALF_CHUNK, CHUNK);
    getHeightIn(HALF_CHUNK, HALF_CHUNK, CHUNK, CHUNK);
}



void WorldMapGenerator::getBiomeMap() {
    auto location = chunk->getLocation();
    for (int x = 0; x < CHUNK_SIZE + 1; x++) {
        for (int z = 0; z < CHUNK_SIZE + 1; z++) {
            int h = biomeNoiseGen.getHeight(x, z, location.x + 10, location.y + 10);
            biomeMap.get(x, z) = h;
        }
    }
}


void WorldMapGenerator::setBlocks(int maxHeight) {
    
    //    std::cout<<">>> set blocks with maxHeight : "<< maxHeight<<std::endl;
    
    std::vector<glm::vec3> trees;
    std::vector<glm::vec3> plants;
    
    for (int y = 0; y < maxHeight + 1; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                
                int height = heightMap.get(x, z);
                auto& biome = getBiome(x, z);
                
                if (y > height) {
                    if (y <= WATER_LEVEL) {
                        chunk->setBlock(BlockId_Water, x, y, z);
                        //                        std::cout<<">>> set block type : [water]  at: ["<<x<<" , "<<y<<" , "<<z<<"]"<<std::endl;
                    }
                    continue;
                }
                else if (y == height) {
                    
                    if (y >= WATER_LEVEL) {
                        
                        if (y < WATER_LEVEL + 4) {
                            
                            BlockId blockId = biome.getBeachBlock(random);
                            chunk->setBlock(blockId, x, y, z);
                            //                            std::cout<<">>> set block type : ["<< getBlockIdNameFrom(blockId)<< "] at: ["<<x<<" , "<<y<<" , "<<z<<"]"<<std::endl;
                            continue;
                        }
                        
                        if (random.intInRange(0, biome.getTreeFrequency()) == 5 ) {
                            trees.emplace_back(x, y + 1, z);
                        }
                        if (random.intInRange(0, biome.getPlantFrequency()) == 5 ) {
                            plants.emplace_back(x, y + 1, z);
                        }
                        BlockId blockId = biome.getTopBlock(random);
                        chunk->setBlock( blockId, x, y, z);
                        //                        std::cout<<">>> set block type : ["<< getBlockIdNameFrom(blockId)<< "] at: ["<<x<<" , "<<y<<" , "<<z<<"]"<<std::endl;
                    }
                    else {
                        BlockId blockId = biome.getUnderWaterBlock(random);
                        chunk->setBlock(blockId, x, y, z );
                        //                        std::cout<<">>> set block type : ["<< getBlockIdNameFrom(blockId)<< "] at: ["<<x<<" , "<<y<<" , "<<z<<"]"<<std::endl;
                    }
                }
                else if (y > height - 3) {
                    chunk->setBlock(BlockId_Dirt, x, y, z);
                    //                    std::cout<<">>> set block type : [Dirt]  at: ["<<x<<" , "<<y<<" , "<<z<<"]"<<std::endl;
                }
                else {
                    chunk->setBlock(BlockId_Stone ,x, y, z);
                    //                    std::cout<<">>> set block type : [Stone]  at: ["<<x<<" , "<<y<<" , "<<z<<"]"<<std::endl;
                }
            }
        }
    }
    
    for (auto& plant : plants) {
        int x = plant.x;
        int z = plant.z;
        auto blockId = getBiome(x, z).getPlant(random);
        chunk->setBlock(blockId, x, plant.y, z);
    }
    
    for (auto& tree : trees) {
        int x = tree.x;
        int z = tree.z;
        getBiome(x, z).makeTree(random,this -> chunk, x, tree.y, z);
    }
}


const Biome& WorldMapGenerator::getBiome(int x, int z) const {
    
    int biomeValue = biomeMap.get(x, z);
    
    if (biomeValue > 160) {
        return grasslandBiome;
    }
    else if (biomeValue > 150) {
        return lightForest;
        
    }
    else if (biomeValue > 130) {
        return grassBiome;
        
    }
    else if (biomeValue > 120) {
        return temperateForest;
    }
    else if (biomeValue > 110) {
        return lightForest;
    }
    else if (biomeValue > 100) {
        return desertBiome;
    }
    else {
        return oceanBiome;
    }
    
}
