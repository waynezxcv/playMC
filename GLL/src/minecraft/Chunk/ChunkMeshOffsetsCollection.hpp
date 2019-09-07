//
//  ChunkMeshOffsetsCollection.hpp
//  GLL
//
//  Created by 刘微 on 2019/9/8.
//  Copyright © 2019 liu.wei. All rights reserved.
//

#ifndef ChunkMeshOffsetsCollection_hpp
#define ChunkMeshOffsetsCollection_hpp

#include <stdio.h>
#include <vector>
#include <array>

#include "Frustum.hpp"
#include "VectorXZ.hpp"
#include "ChunkDefine.hpp"


namespace GLL {
    
    class ChunkMeshOffsetsCollection {
    public:
        AABB aabb;
        int sectionIndex;
        std::vector<glm::vec3> offsets;
        
    public:
        ChunkMeshOffsetsCollection(const int& index);
        void clear();
        int& getSectionIndex();
        std::vector<glm::vec3>& getMeshOffsets();
    };
    
    
    class ChunkMeshSectionCollection {
    public:
        std::array<std::shared_ptr<ChunkMeshOffsetsCollection>, CHUNK_SIZE> sections;
        VectorXZ xz;
        
    public:
        ChunkMeshSectionCollection(const VectorXZ& xz);
        void clear();
        std::array<std::shared_ptr<ChunkMeshOffsetsCollection>, CHUNK_SIZE>& getSectionsArray();
        VectorXZ& getXZ();
        
    };
    
}

#endif /* ChunkMeshOffsetsCollection_hpp */
