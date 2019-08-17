


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


namespace GLL {
    
    class InstanceMeshDrawable {
    public:
        InstanceMeshDrawable(const BlockDataContent& blockData ,const ChunkMesh::ChunkMeshFaceDirection& direction);
        ~InstanceMeshDrawable();
        void addOffsetIfNeeded(const glm::vec3& offset);
        void instanceDraw(std::shared_ptr<Camera> camera, std::shared_ptr<FrameBuffer> frameBuffer);
        BlockDataContent getBlockData() const;
        
    private:
        std::atomic<bool> dataBuffered;
        std::atomic<long> currentBuffredOffsetsCount;
        std::recursive_mutex offsetsMutex;
        
        GLuint VAO, VBO, EBO = 0;
        GLuint instanceVBO = 0;
        std::vector<glm::vec3> offsets;
        BlockDataContent blockData;
        ChunkMesh::ChunkMeshFaceDirection direction;
        std::vector<ChunkMesh::ChunkMeshVertex> vertices;
        
    private:
        void makeVertices(const std::vector<glm::vec3>& face, const std::vector<glm::vec2>& texCoords, const GLfloat& cardinalLight);
        void bufferData();
        void bufferInstanceSubData();
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