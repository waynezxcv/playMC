

#include "InstanceMeshDrawable.hpp"
#include "BlockDataFactory.hpp"


using namespace GLL;

namespace  {
    constexpr GLuint MAX_OFFSETS_DATA_SIZE = 1024 * 1024 * 5; // 5MB
}

InstanceMeshDrawable::InstanceMeshDrawable(const BlockDataContent& blockData ,const ChunkMesh::ChunkMeshFaceDirection& direction) :
blockData(blockData),
direction(direction)
{
    currentBuffredOffsetsCount = 0;
}

InstanceMeshDrawable::~InstanceMeshDrawable() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
    }
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
    }
    if (instanceVBO != 0) {
        glDeleteBuffers(1, &instanceVBO);
    }
}

void InstanceMeshDrawable::bufferData() {
    switch (direction) {
        case ChunkMesh::ChunkMeshFaceDirection_PositiveY: {
            makeVertices(topFace,TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.topTextureCoords), LIGHT_TOP);
        }
            break;
            
        case ChunkMesh::ChunkMeshFaceDirection_NegativeY: {
            makeVertices(bottomFace, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.bottomTextureCoords), LIGHT_BOT);
        }
            
            break;
            
        case ChunkMesh::ChunkMeshFaceDirection_NegativeX: {
            makeVertices(leftFace, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.sideTextrueCoords), LIGHT_X);
        }
            
            break;
            
        case ChunkMesh::ChunkMeshFaceDirection_PositiveX: {
            makeVertices(rightFace,TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.sideTextrueCoords), LIGHT_X);
            
        }
            break;
            
        case ChunkMesh::ChunkMeshFaceDirection_PositiveZ: {
            makeVertices(frontFace, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.sideTextrueCoords), LIGHT_Z);
        }
            break;
            
        case ChunkMesh::ChunkMeshFaceDirection_NegativeZ: {
            makeVertices(backFace, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.sideTextrueCoords), LIGHT_Z);
        }
            
            break;
            
        case ChunkMesh::ChunkMeshFaceDirection_XZ: {
            makeVertices(xFace1, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.topTextureCoords), LIGHT_X);
        }
            break;
            
        case ChunkMesh::ChunkMeshFaceDirection_ZX: {
            makeVertices(xFace2, TextureAtlas::sharedInstance().textureCoordsFromTexutreIndex(blockData.topTextureCoords), LIGHT_X);
        }
            break;
    }
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanceVBO);
    
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
    
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MAX_OFFSETS_DATA_SIZE, NULL, GL_STATIC_DRAW);
    
    this -> offsetsMutex.lock();
    GLint offsetsSzie = (GLint)offsets.size();
    this -> offsetsMutex.unlock();
    
    if (currentBuffredOffsetsCount != offsetsSzie) {
        this -> bufferInstanceSubData();
    }
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *) (0));
    glVertexAttribDivisor(4, 1);
    glBindVertexArray(0);
    
    std::cout<<">> buffered data for instance :[ " << this -> blockData.blockId<<this->direction<<" ]"<<std::endl;
}

void InstanceMeshDrawable::bufferInstanceSubData() {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    this -> offsetsMutex.lock();
    glBufferSubData(GL_ARRAY_BUFFER, currentBuffredOffsetsCount * sizeof(glm::vec3), (offsets.size() - currentBuffredOffsetsCount) * sizeof(glm::vec3), &offsets[currentBuffredOffsetsCount]);
    currentBuffredOffsetsCount = offsets.size();
    this -> offsetsMutex.unlock();
    std::cout<<">> buffered instance sub data for instance : [ " << this -> blockData.blockId<<this->direction<<" ]" <<"current offsets size :"<<currentBuffredOffsetsCount<<std::endl;
    
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


void InstanceMeshDrawable::instanceDraw(std::shared_ptr<Camera> camera, std::shared_ptr<FrameBuffer> frameBuffer) {
    
    if (dataBuffered == false) {
        bufferData();
        dataBuffered = true;
    }
    
    this -> offsetsMutex.lock();
    GLint offsetsSzie = (GLint)offsets.size();
    this -> offsetsMutex.unlock();
    
    if (currentBuffredOffsetsCount != offsetsSzie) {
        this -> bufferInstanceSubData();
    }
    
    TextureAtlas::sharedInstance().bindTexture();
#if POLYGON_LINE_ENAGLED
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0, (int)offsetsSzie);
    glBindVertexArray(0);
#if POLYGON_LINE_ENAGLED
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    TextureAtlas::sharedInstance().unbindTexture();
}

bool InstanceMeshDrawable::addOffsetIfNeeded(const glm::vec3& offset) {
    this -> offsetsMutex.lock();
    bool result = false;
    if (std::find(offsets.begin(), offsets.end(), offset) == offsets.end()) {
        offsets.push_back(offset);
        result = true;
    }
    if (offsets.size() > MAX_OFFSETS_DATA_SIZE) {
        std::cout<<"[ERROR] the offsets is heap over flow ... "<<std::endl;
    }
    this -> offsetsMutex.unlock();
    return result;
}


BlockDataContent InstanceMeshDrawable::getBlockData() const {
    return this -> blockData;
}


int InstanceMeshDrawable::getCurrentOffsetSize() const {
    this -> offsetsMutex.lock();
    auto count = this -> offsets.size();
    this -> offsetsMutex.unlock();
    return count;
}
