
#ifndef StructureBuilder_hpp
#define StructureBuilder_hpp

#include <stdio.h>
#include <vector>
#include "BlockData.hpp"



namespace GLL {
    class Chunk;
    
    class StructureBuilder {
        struct Block {
            Block(BlockId id, int x, int y, int z) : id (id), x (x) , y (y), z (z) {};
            BlockId id;
            int x, y, z;
        };
        
        
    public:
        void build(std::shared_ptr<Chunk>);
        void makeColumn(int x, int z, int yStart, int height, BlockId block);
        void makeRowX(int xStart, int xEnd, int y, int z, BlockId block);
        void makeRowZ(int zStart, int zEnd, int x, int y, BlockId block);
        void fill(int y, int xStart, int xEnd, int zStart, int zEnd, BlockId block);
        void addBlock(int x, int y, int z, BlockId block);
    private:
        std::vector<Block> blocks;
    };
}


#endif /* StructureBuilder_hpp */
