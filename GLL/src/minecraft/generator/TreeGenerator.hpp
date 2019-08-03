
#ifndef TreeGenerator_hpp
#define TreeGenerator_hpp

#include <stdio.h>
#include "RandomSingleton.hpp"

namespace GLL {
    class Chunk;
    void makeOakTree (Chunk& chunk, Random<std::minstd_rand>& rand, int x, int y, int z);
    void makePalmTree (Chunk& chunk, Random<std::minstd_rand>& rand, int x, int y, int z);
    void makeCactus (Chunk& chunk, Random<std::minstd_rand>& rand, int x, int y, int z);
}

#endif /* TreeGenerator_hpp */
