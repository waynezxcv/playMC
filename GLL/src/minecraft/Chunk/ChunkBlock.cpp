

#include "ChunkBlock.hpp"
#include "ChunkSection.hpp"
#include "Chunk.hpp"

using namespace GLL;

#pragma mark - LifeCycle

ChunkBlock::ChunkBlock(std::weak_ptr<ChunkSection> parentSection,
                       const BlockId& blockId,
                       const glm::vec3& blockPositionInSection ,
                       const glm::vec3& blockPositionInWorld) :

parentSection(parentSection),
blockData(BlockDataFactory::sharedInstance() -> getBlockDataConent(blockId)) ,
blockPositionInSection(blockPositionInSection) ,
blockPositonInWorld(blockPositionInWorld) {
    
}

ChunkBlock::~ChunkBlock() {
    
}

#pragma mark - Public Interface

void ChunkBlock::updateBlockId(const BlockId& blockId) {
    BlockDataContent origData = blockData;
    if (origData.blockId == blockId) {
        return;
    }
    blockData = BlockDataFactory::sharedInstance() -> getBlockDataConent(blockId);
    this -> getParentSecton() -> setNeedUpdate();
}

#pragma mark - Getter

glm::vec3 ChunkBlock::getBlockPositonInWorld() const {
    return blockPositonInWorld;
}

glm::vec3 ChunkBlock::getBlockPositionInSection() const {
    return blockPositionInSection;
}

BlockDataContent ChunkBlock::getBlockData() const {
    return blockData;
}

std::shared_ptr<ChunkSection> ChunkBlock::getParentSecton() {
    if (std::shared_ptr<ChunkSection> sp  = this -> parentSection.lock()) {
        return sp;
    }
    else {
        return nullptr;
    }
}
