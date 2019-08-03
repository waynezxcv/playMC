

#ifndef PerlinNoise_hpp
#define PerlinNoise_hpp

#include <stdio.h>

namespace GLL {
    
    struct NoiseParameters {
        int octaves;
        int amplitude;
        int smoothness;
        int heightOffset;
        double roughness;
    };
    
    class PerlinNoise {
        
    public:
        PerlinNoise(int seed);
        double getHeight(int x, int z, int chunkX, int chunkZ) const noexcept;
        void setParameters(const NoiseParameters& params) noexcept;
        
    private:
        double getNoise(int  n) const noexcept;
        double getNoise(double  x, double  z) const noexcept;
        double lerp(double a, double b, double z) const noexcept;
        double noise(double  x, double  z) const noexcept;
        NoiseParameters noiseParameters;
        int seed;
    };
}

#endif /* PerlinNoise_hpp */
