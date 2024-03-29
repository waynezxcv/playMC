
#include "PerlinNoise.hpp"
#include "ChunkDefine.hpp"

using namespace GLL;



PerlinNoise::PerlinNoise(int seed) :
seed (seed) {
    noiseParameters.octaves = 7;
    noiseParameters.amplitude = 70;
    noiseParameters.smoothness = 235;
    noiseParameters.heightOffset = -5;
    noiseParameters.roughness = 0.53;
}

void PerlinNoise::setParameters(const NoiseParameters& params) noexcept {
    noiseParameters = params;
}

double PerlinNoise::getNoise(int  n) const noexcept {
    n += seed;
    n = (n << 13) ^ n;
    auto newN = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
    return 1.0 - ((double)newN / 1073741824.0);
}


double PerlinNoise::getNoise(double  x, double  z) const noexcept {
    return getNoise(x + z * 57);
}


double PerlinNoise::lerp(double a, double b, double z) const noexcept {
    double mu2 = (1 - std::cos(z * 3.14)) / 2;
    return (a * (1 - mu2) + b * mu2);
}


double PerlinNoise::noise(double  x, double  z) const noexcept {
    
    auto floorX = (double)((int)x);
    auto floorZ = (double)((int)z);
    
    auto s = 0.0,
    t = 0.0,
    u = 0.0,
    v = 0.0;
    s = getNoise(floorX, floorZ);
    t = getNoise(floorX + 1, floorZ);
    u = getNoise(floorX, floorZ + 1);
    v = getNoise(floorX + 1, floorZ + 1);
    
    auto rec1 = lerp(s, t, x - floorX);
    auto rec2 = lerp(u, v, x - floorX);
    auto rec3 = lerp(rec1, rec2, z - floorZ);
    return rec3;
}


double PerlinNoise::getHeight(int x, int z, int chunkX, int chunkZ) const noexcept {
    
    auto newX = (x + (chunkX * CHUNK_SIZE));
    auto newZ = (z + (chunkZ * CHUNK_SIZE));
    
    if (newX < 0 || newZ < 0) {
        return WATER_LEVEL - 1;
    }
    
    auto totalValue = 0.0;
    
    for (auto a = 0; a < noiseParameters.octaves - 1; a++ ) {
        auto frequency = pow(2.0, a);
        auto amplitude = pow(noiseParameters.roughness, a);
        totalValue += noise(((double)newX) * frequency / noiseParameters.smoothness, ((double)newZ) * frequency / noiseParameters.smoothness) * amplitude;
    }
    auto val = (((totalValue / 2.1) + 1.2) * noiseParameters.amplitude) + noiseParameters.heightOffset;
    return val > 0 ? val : 1;
}



