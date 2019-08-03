
#ifndef BlockDataFactory_hpp
#define BlockDataFactory_hpp

#include <stdio.h>
#include "NonCopyable.hpp"
#include "BlockData.hpp"
#include "TextureAtlas.hpp"


/*
 block工厂
 */

namespace GLL {
    class BlockDataFactory : public NonCopyable {
    public:
        
    public:
        
        static BlockDataFactory* sharedInstance () {
            static BlockDataFactory instance;
            return &instance;
        }
        
        const BlockData& getBlockData(const BlockId& id) const;
        const BlockDataContent& getBlockDataConent(const BlockId& id) const;
        
    private:
        BlockDataFactory();
        std::array<std::unique_ptr<BlockData>, BlockId_Count> blocks;
    };
}


#endif /* BlockDataFactory_hpp */
