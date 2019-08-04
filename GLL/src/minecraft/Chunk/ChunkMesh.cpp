
#include "ChunkMesh.hpp"


using namespace GLL;


ChunkMesh::ChunkMesh(const BlockDataContent& blockData ,const ChunkMeshFaceDirection& directon, const glm::vec3& position) :
direction(directon),
position(position),
blockData(blockData) {
}


ChunkMesh::~ChunkMesh() {
    deleteMeshBuffer();
}

void ChunkMesh::makeMesh() {
    switch (direction) {
        case ChunkMeshFaceDirection_PositiveY: {
            
            makeVertices(topFace, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.topTextureCoords), LIGHT_TOP, position);
        }
            break;
            
        case ChunkMeshFaceDirection_NegativeY: {
            makeVertices(bottomFace, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.bottomTextureCoords), LIGHT_BOT, position);
        }
            
            break;
            
        case ChunkMeshFaceDirection_NegativeX: {
            makeVertices(leftFace, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.sideTextrueCoords), LIGHT_X, position);
        }
            
            break;
            
        case ChunkMeshFaceDirection_PositiveX: {
            makeVertices(rightFace, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.sideTextrueCoords), LIGHT_X, position);
            
        }
            break;
            
        case ChunkMeshFaceDirection_PositiveZ: {
            makeVertices(frontFace, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.sideTextrueCoords), LIGHT_Z, position);
        }
            break;
            
        case ChunkMeshFaceDirection_NegativeZ: {
            makeVertices(backFace, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.sideTextrueCoords), LIGHT_Z, position);
        }
            
            break;
            
        case ChunkMeshFaceDirection_XZ: {
            makeVertices(xFace1, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.topTextureCoords), LIGHT_X, position);
        }
            break;
            
        case ChunkMeshFaceDirection_ZX: {
            makeVertices(xFace2,TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.topTextureCoords), LIGHT_X, position);
        }
            break;
    }
}


void ChunkMesh::makeVertices( const std::vector<glm::vec3>& face, const std::vector<glm::vec2>& texCoords, const GLfloat& cardinalLight, const glm::vec3& position) {
    for (int i = 0; i <  4; i ++) {
        ChunkMeshVertex vertex;
        vertex.vertextPosition = glm::vec3 { face[i].x + position.x , face[i].y + position.y, face[i].z + position.z};
        vertex.texCoords = texCoords[i];
        vertex.normal = glm::vec3 {1.0f}; // TODO：
        vertex.cardinalLight = cardinalLight;
        vertices.push_back(vertex);
    }
}

void ChunkMesh::bufferMesh() {
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ChunkMesh::ChunkMeshVertex) * getVertices().size(), getVertices().data(), GL_STATIC_DRAW);
    
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
    
    glBindVertexArray(0);
}

void ChunkMesh::deleteMeshBuffer() {
    if (VAO != 0) {
        glDeleteBuffers(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
}

void ChunkMesh::bindMeshVAO() {
    glBindVertexArray(VAO);
}


std::vector<GLL::ChunkMesh::ChunkMeshVertex> ChunkMesh::getVertices() const {
    return this -> vertices;
}

GLL::ChunkMesh::ChunkMeshFaceDirection ChunkMesh::getDirection() const {
    return this -> direction;
}

BlockDataContent ChunkMesh::getBlockData() const {
    return this -> blockData;
}

glm::vec3 ChunkMesh::getPosition() const {
    return this -> position;
}
