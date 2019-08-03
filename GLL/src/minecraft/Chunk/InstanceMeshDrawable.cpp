

#include "InstanceMeshDrawable.hpp"
#include "BlockDataFactory.hpp"


using namespace GLL;

InstanceMeshDrawable::InstanceMeshDrawable(const BlockDataContent& blockData ,const ChunkMesh::ChunkMeshFaceDirection& direction) :
blockData(blockData),
direction(direction)
{
}


InstanceMeshDrawable::~InstanceMeshDrawable() {
    if (instanceVBO != 0) {
        glDeleteBuffers(1, &instanceVBO);
    }
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
    }
    
    if (VBO != 0) {
        glDeleteVertexArrays(1, &VBO);
    }
}

void InstanceMeshDrawable::bufferData() {
    switch (direction) {
        case ChunkMesh::ChunkMeshFaceDirection_PositiveY: // top
        {
            makeVertices(topFace,TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.topTextureCoords), LIGHT_TOP);
        }
            break;
            
        case ChunkMesh::ChunkMeshFaceDirection_NegativeY: // bottom
        {
            makeVertices(bottomFace, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.bottomTextureCoords), LIGHT_BOT);
        }
            
            break;
            
        case ChunkMesh::ChunkMeshFaceDirection_NegativeX: //left
        {
            makeVertices(leftFace, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.sideTextrueCoords), LIGHT_X);
        }
            
            break;
            
        case ChunkMesh::ChunkMeshFaceDirection_PositiveX: //right
        {
            makeVertices(rightFace,TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.sideTextrueCoords), LIGHT_X);
            
        }
            break;
            
        case ChunkMesh::ChunkMeshFaceDirection_PositiveZ: // front
        {
            makeVertices(frontFace, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.sideTextrueCoords), LIGHT_Z);
        }
            break;
            
        case ChunkMesh::ChunkMeshFaceDirection_NegativeZ: //back
        {
            makeVertices(backFace, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.sideTextrueCoords), LIGHT_Z);
        }
            
            break;
            
            
        case ChunkMesh::ChunkMeshFaceDirection_XZ: // xface1
        {
            makeVertices(xFace1, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.topTextureCoords), LIGHT_X);
        }
            break;
            
        case ChunkMesh::ChunkMeshFaceDirection_ZX: // xface2
        {
            makeVertices(xFace2, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.topTextureCoords), LIGHT_X);
        }
            break;
    }
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ChunkMesh::ChunkMeshVertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void *) (0));
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void *) (sizeof(GLfloat) * 3));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void *) (sizeof(GLfloat) * 5));
    
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void *) (sizeof(GLfloat) * 6));
    
    
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * offsets.size(), offsets.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *) (0));
    glVertexAttribDivisor(4, 1);
    
    glBindVertexArray(0);
}


void InstanceMeshDrawable::makeVertices(const std::vector<glm::vec3>& face, const std::vector<glm::vec2>& texCoords, const GLfloat& cardinalLight) {
    for (int i = 0; i <  4; i ++) {
        ChunkMesh::ChunkMeshVertex vertex;
        vertex.vertextPosition = glm::vec3{face[i].x, face[i].y, face[i].z};
        vertex.texCoords = texCoords[i];
        vertex.normal = glm::vec3 {1.0f}; // TODO：
        vertex.cardinalLight = cardinalLight;
        vertices.push_back(vertex);
    }
}

void InstanceMeshDrawable::instanceDraw(Camera *camera, std::shared_ptr<FrameBuffer> frameBuffer) {
    
    if (dataBuffered == false) {
        bufferData();
        dataBuffered = true;
    }

    TextureAtlas::sharedInstance().bindTexture();
#if POLYGON_LINE_ENAGLED
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
    
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0, (int) this -> offsets.size());
    glBindVertexArray(0);
    
#if POLYGON_LINE_ENAGLED
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    TextureAtlas::sharedInstance().unbindTexture();
}


void InstanceMeshDrawable::addOffset(const glm::vec3& offset) {
    offsets.push_back(offset);
}


GLuint InstanceMeshDrawable::getOffsetsSize() const {
    return (GLuint)offsets.size();
}


BlockDataContent InstanceMeshDrawable::getBlockData() const {
    return this -> blockData;
}
