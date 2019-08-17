
#ifndef ChunkMesh_hpp
#define ChunkMesh_hpp

#include <stdio.h>
#include <vector>
#include "Shader.hpp"
#include "BlockDataFactory.hpp"
#include "ChunkDefine.hpp"




namespace GLL {
    
    // 逆时针表示正面
    const std::vector<GLuint> indices {
        0 , 1, 2,
        2 , 3, 0,
    };
        
    const std::vector<glm::vec3> frontFace { //check
        glm::vec3{0, 0, 1,},
        glm::vec3{1, 0, 1,},
        glm::vec3{1, 1, 1,},
        glm::vec3{0, 1, 1,},
    };
    
    const std::vector<glm::vec3> backFace {
        glm::vec3{1, 0, 0,},
        glm::vec3{0, 0, 0,},
        glm::vec3{0, 1, 0,},
        glm::vec3{1, 1, 0,},
        
    };
    
    const std::vector<glm::vec3> leftFace {
        glm::vec3{0, 0, 0,},
        glm::vec3{0, 0, 1,},
        glm::vec3{0, 1, 1,},
        glm::vec3{0, 1, 0,},
        
    };
    
    const std::vector<glm::vec3> rightFace {
        glm::vec3{1, 0, 1,},
        glm::vec3{1, 0, 0,},
        glm::vec3{1, 1, 0,},
        glm::vec3{1, 1, 1,},
    };
    
    const std::vector<glm::vec3> topFace { //check
        glm::vec3{ 0, 1, 1,},
        glm::vec3{ 1, 1, 1,},
        glm::vec3{ 1, 1, 0,},
        glm::vec3{0, 1, 0,},
        
    };
    
    const std::vector<glm::vec3> bottomFace {
        glm::vec3{ 0, 0, 0,},
        glm::vec3{ 1, 0, 0,},
        glm::vec3{ 1, 0, 1,},
        glm::vec3{ 0, 0, 1},
    };
    
    
    const std::vector<glm::vec3> xFace1 {
        glm::vec3{ 0, 0, 0,},
        glm::vec3{ 1, 0, 1,},
        glm::vec3{ 1, 1, 1, },
        glm::vec3{ 0, 1, 0, },
    };
    
    
    const std::vector<glm::vec3> xFace2 {
        glm::vec3{ 0, 0, 1, },
        glm::vec3{  1, 0, 0, },
        glm::vec3{ 1, 1, 0, },
        glm::vec3{  0, 1, 1, },
    };
    
    constexpr GLfloat LIGHT_TOP = 1.0f;
    constexpr GLfloat LIGHT_X = 0.8f;
    constexpr GLfloat LIGHT_Z = 0.6f;
    constexpr GLfloat LIGHT_BOT = 0.4f;
    
    
    class ChunkMesh {
    public:
        typedef enum : int {
            ChunkMeshFaceDirection_PositiveY = 0,
            ChunkMeshFaceDirection_NegativeY,
            ChunkMeshFaceDirection_PositiveX,
            ChunkMeshFaceDirection_NegativeX,
            ChunkMeshFaceDirection_PositiveZ,
            ChunkMeshFaceDirection_NegativeZ,
            ChunkMeshFaceDirection_XZ,
            ChunkMeshFaceDirection_ZX
        } ChunkMeshFaceDirection;
        
        typedef struct {
            glm::vec3 vertextPosition;
            glm::vec2 texCoords;
            GLfloat cardinalLight;
            glm::vec3 normal;
        } ChunkMeshVertex;
        
    public:
        ChunkMesh(const BlockDataContent& blockData ,const ChunkMeshFaceDirection& directon, const glm::vec3& position);
        ~ChunkMesh();
        
    public:
        void makeMesh();
        void bufferMesh();
        void deleteMeshBuffer();
        void bindMeshVAO();
        
    public:
        BlockDataContent getBlockData() const;
        ChunkMeshFaceDirection getDirection() const;
        
        std::vector<ChunkMeshVertex> getVertices() const;
        glm::vec3 getPosition() const;
    private:
        BlockDataContent blockData;
        ChunkMeshFaceDirection direction;
        glm::vec3 position;
        std::vector<ChunkMeshVertex> vertices;
        GLuint VAO, VBO, EBO = 0;
    private:
        void setup();
        void makeVertices(const std::vector<glm::vec3>& face, const std::vector<glm::vec2>& texCoords, const GLfloat& cardinalLight, const glm::vec3& position);
    };
}

#endif /* ChunkMesh_hpp */
