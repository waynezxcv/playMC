
#include "BlockData.hpp"
#include <fstream>
#include "FileManager.hpp"


using namespace GLL;


BlockData::BlockData(const std::string& blockPath) {
    
    std::string finalPath;
    finalPath = FileManager::getAssetDirectory() + "block/"+blockPath+".block";
    std::ifstream inFile(finalPath);
    
    
    if (!inFile.is_open()) {
        throw std::runtime_error("unbalbe to open block file ... at" + finalPath);
    }
    
    
    std::string line;
    while (std::getline(inFile, line)) {
        
        if (line == "TexTop") {
            int x, y;
            inFile >> x >> y;
            data.topTextureCoords = {x, y};
        }
        
        else if (line == "TexBottom") {
            int x, y;
            inFile >> x >> y;
            data.bottomTextureCoords = {x, y};
        }
        
        else if (line == "TexSide") {
            int x, y;
            inFile >> x >> y;
            data.sideTextrueCoords = {x, y};
        }
        
        else if (line == "TexAll") {
            int x, y;
            inFile >> x >> y;
            data.topTextureCoords = {x, y};
            data.sideTextrueCoords = {x, y};
            data.bottomTextureCoords = {x, y};
        }
        
        else if (line == "Id") {
            int id;
            inFile >> id;
            data.blockId = static_cast<BlockId> (id);
        }
        
        else if (line == "MeshType") {
            int id;
            inFile >> id;
            data.meshType = static_cast<BlockMeshType>(id);
        }
        
        else if (line == "ShaderType") {
            int id;
            inFile >> id;
            data.shaderType = static_cast<BlockShaderType>(id);
        }
        
        else if (line == "Opaque") {
            inFile >> data.isOpaque;
        }
        
        else if (line == "Collidable") {
            inFile >> data.isCollidable;
        }
        
        else if (line == "Name") {
            inFile >> data.blockName;
        }
        
    }
}


BlockData::~BlockData() {}


const BlockDataContent& BlockData::getDataContent() const {
    return this -> data;
}
