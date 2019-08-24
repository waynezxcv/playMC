
#ifndef TreeGenerator_hpp
#define TreeGenerator_hpp

#include <stdio.h>
#include "RandomSingleton.hpp"

namespace GLL {
    class Chunk;
    void makeOakTree (std::shared_ptr<Chunk>, Random<std::minstd_rand>& rand, int x, int y, int z);
    void makePalmTree (std::shared_ptr<Chunk>, Random<std::minstd_rand>& rand, int x, int y, int z);
    void makeCactus (std::shared_ptr<Chunk>, Random<std::minstd_rand>& rand, int x, int y, int z);
}

#endif /* TreeGenerator_hpp */
