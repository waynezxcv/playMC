


#ifndef InstanceMeshDrawable_hpp
#define InstanceMeshDrawable_hpp

#include <stdio.h>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>

#include "Camera.hpp"
#include "FrameBuffer.hpp"
#include "BlockData.hpp"
#include "Chunk.hpp"
#include "ChunkMeshOffsetsCollection.hpp"



namespace GLL {
    using OffsetsMapType = std::unordered_map<std::string, std::shared_ptr<ChunkMeshSectionCollection>>;
    class InstanceMeshDrawable {
    public:
        InstanceMeshDrawable(const BlockDataContent& blockData ,const ChunkMesh::ChunkMeshFaceDirection& direction);
        ~InstanceMeshDrawable();
        
        void addMeshOffsets(const VectorXZ& xz, const int& sectionIndex, const AABB& aabb,  std::vector<glm::vec3> offsetsVector);
        
        void clearOffsetsFor(VectorXZ&& xz);
        void clearAll();
        
        void instanceDraw(std::shared_ptr<Camera> camera, std::shared_ptr<FrameBuffer> frameBuffer);
        BlockDataContent getBlockData() const;
        
    private:
        BlockDataContent blockData;
        ChunkMesh::ChunkMeshFaceDirection direction;
        std::vector<ChunkMesh::ChunkMeshVertex> vertices;
        
        GLuint VAO, VBO, EBO = 0;
        GLuint instanceVBO = 0;
        std::atomic<bool> vaoDataBuffered{false};
        std::atomic<int> combineOffsetsSize{0};
        
        OffsetsMapType offsetsMap;
        std::mutex mutex;
        
    private:
        void makeVertices(const std::vector<glm::vec3>& face, const std::vector<glm::vec2>& texCoords, const GLfloat& cardinalLight);
        void bufferVaoData();
        std::string getKey(VectorXZ xz);
    };
}

namespace GLL {
    class InstanceMeshDrawableFactory {
    public:
        static std::unique_ptr<InstanceMeshDrawable> makeInstanceMeshDrawable(const BlockDataContent& blockData ,const ChunkMesh::ChunkMeshFaceDirection& direction) {
            return std::make_unique<InstanceMeshDrawable>(blockData, direction);
        }
    };
}


#endif /* InstanceMeshDrawable_hpp */
