
#ifndef BlockData_hpp
#define BlockData_hpp

#include <stdio.h>
#include "FileManager.hpp"


/*
 block模型
 */

namespace GLL {
    
    typedef enum : int {
        BlockId_Air = 0,
        BlockId_Grass = 1,
        BlockId_Dirt = 2,
        BlockId_Stone = 3,
        BlockId_Oakbark = 4,
        BlockId_OakLeaft = 5,
        BlockId_Sand = 6,
        BlockId_Water = 7,
        BlockId_Cactus = 8,
        BlockId_Rose = 9,
        BlockId_TallGrass = 10,
        BlockId_DeadShrub = 11,
        BlockId_Count = 12,
    } BlockId;
    
    
    typedef enum : int {
        BlockShaderType_Chunck = 0,
        BlockShaderType_Liquid = 1,
        BlockShaderType_Flora = 2,
    } BlockShaderType;
    
    
    typedef enum  : int {
        BlockMeshType_Cube = 0,
        BlockMeshType_X = 1,
    } BlockMeshType;
    
    
    struct BlockDataContent {
        
        std::string blockName;
        BlockId blockId;
        BlockMeshType meshType;
        BlockShaderType shaderType;
        
        glm::vec2 topTextureCoords;
        glm::vec2 sideTextrueCoords;
        glm::vec2 bottomTextureCoords;
        
        bool isOpaque;
        bool isCollidable;
    };
    
    
    class BlockData {
    public:
        
        explicit BlockData(const std::string& blockPath);
        ~BlockData();
        const BlockDataContent& getDataContent() const;
        
    private:
        BlockDataContent data;
    private:
        
    };
}


#endif /* BlockData_hpp */
