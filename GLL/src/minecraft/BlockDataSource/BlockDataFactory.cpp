
#include "BlockDataFactory.hpp"


using namespace GLL;

BlockDataFactory::BlockDataFactory()
{
    blocks[(int)BlockId_Air] = std::make_unique<BlockData>("Air");
    blocks[(int)BlockId_Grass] = std::make_unique<BlockData>("Grass");
    blocks[(int)BlockId_Dirt] = std::make_unique<BlockData>("Dirt");
    blocks[(int)BlockId_Stone] = std::make_unique<BlockData>("Stone");
    blocks[(int)BlockId_Oakbark] = std::make_unique<BlockData>("OakBark");
    blocks[(int)BlockId_OakLeaft] = std::make_unique<BlockData>("OakLeaf");
    blocks[(int)BlockId_Sand] = std::make_unique<BlockData>("Sand");
    blocks[(int)BlockId_Water] = std::make_unique<BlockData>("Water");
    blocks[(int)BlockId_Cactus] = std::make_unique<BlockData>("Cactus");
    blocks[(int)BlockId_TallGrass] = std::make_unique<BlockData>("TallGrass");
    blocks[(int)BlockId_Rose] = std::make_unique<BlockData>("Rose");
    blocks[(int)BlockId_DeadShrub] = std::make_unique<BlockData>("DeadShrub");
}

const BlockData& BlockDataFactory::getBlockData(const BlockId& id) const {
    return *(this -> blocks[(int)id]);
}

const BlockDataContent& BlockDataFactory::getBlockDataConent(const BlockId& id) const {
    return this -> blocks[(int)id] -> getDataContent();
}

