


#ifndef InstanceMeshDrawable_hpp
#define InstanceMeshDrawable_hpp

#include <stdio.h>
#include <vector>
#include "Camera.hpp"
#include "FrameBuffer.hpp"
#include "BlockData.hpp"
#include "Chunk.hpp"


namespace GLL {
    
    class InstanceMeshDrawable {
    public:
        InstanceMeshDrawable(const BlockDataContent& blockData ,const ChunkMesh::ChunkMeshFaceDirection& direction);
        ~InstanceMeshDrawable();
        void addOffset(const glm::vec3& offset);
        void instanceDraw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer);
        GLuint getOffsetsSize() const;
        BlockDataContent getBlockData() const;
        
    private:
        bool dataBuffered = false;
        GLuint VAO, VBO, EBO = 0;
        GLuint instanceVBO = 0;
        std::vector<glm::vec3> offsets;
        void bufferData();
        
        BlockDataContent blockData;
        ChunkMesh::ChunkMeshFaceDirection direction;
        std::vector<ChunkMesh::ChunkMeshVertex> vertices;

        void makeVertices(const std::vector<glm::vec3>& face, const std::vector<glm::vec2>& texCoords, const GLfloat& cardinalLight);
    };
}

#endif /* InstanceMeshDrawable_hpp */
