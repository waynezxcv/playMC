//
//  ChunkMeshOffsetsCollection.cpp
//  GLL
//
//  Created by 刘微 on 2019/9/8.
//  Copyright © 2019 liu.wei. All rights reserved.
//

#include "ChunkMeshOffsetsCollection.hpp"

using namespace GLL;

ChunkMeshOffsetsCollection::ChunkMeshOffsetsCollection(const int& index) : sectionIndex(index) , aabb({CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE}) {
    
}

void ChunkMeshOffsetsCollection::clear() {
    this -> offsets.clear();
}


int& ChunkMeshOffsetsCollection::getSectionIndex() {
    return this -> sectionIndex;
}


std::vector<glm::vec3>& ChunkMeshOffsetsCollection::getMeshOffsets() {
    return this -> offsets;
}



ChunkMeshSectionCollection::ChunkMeshSectionCollection(const VectorXZ& xz) : xz(xz) {
    for (int i = 0; i < CHUNK_SIZE; i ++) {
        std::shared_ptr<ChunkMeshOffsetsCollection> item = std::make_shared<ChunkMeshOffsetsCollection>(i);
        this -> sections.at(i) = (std::move(item));
    }
}

void ChunkMeshSectionCollection::clear() {
    for (auto& one : sections) {
        one -> clear();
    }
}

std::array<std::shared_ptr<ChunkMeshOffsetsCollection>, CHUNK_SIZE>& ChunkMeshSectionCollection::getSectionsArray() {
    return this -> sections;
}


VectorXZ& ChunkMeshSectionCollection::getXZ() {
    return this -> xz;
}
