

#include "InstanceMeshDrawable.hpp"
#include "BlockDataFactory.hpp"
#include "ChunkManager.hpp"



using namespace GLL;

namespace  {
    constexpr GLuint MAX_OFFSETS_DATA_SIZE = 1024 * 1024 * 5; // 5MB
}

InstanceMeshDrawable::InstanceMeshDrawable(const BlockDataContent& blockData ,const ChunkMesh::ChunkMeshFaceDirection& direction) :
blockData(blockData),
direction(direction)
{
}


BlockDataContent InstanceMeshDrawable::getBlockData() const {
    return this -> blockData;
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

void InstanceMeshDrawable::bufferVaoData() {
    
    vaoDataBuffered = true;
    
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
    
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *) (0));
    glVertexAttribDivisor(4, 1);
    glBindVertexArray(0);
}

void InstanceMeshDrawable::addMeshOffsets(const VectorXZ& xz, const int& sectionIndex, const AABB& aabb,  std::vector<glm::vec3> offsetsVector) {
    std::lock_guard<std::mutex> lock(this -> mutex);
    std::string key = getKey(xz);
    bool found = this -> offsetsMap.find(key) != offsetsMap.end();
    if (found) {
        std::shared_ptr<ChunkMeshSectionCollection> sectionCollecton = offsetsMap.at(key);
        sectionCollecton -> sections[sectionIndex] -> aabb = aabb;
        sectionCollecton -> sections[sectionIndex] -> getMeshOffsets().insert(sectionCollecton -> sections[sectionIndex] -> offsets.end(), offsetsVector.begin(), offsetsVector.end());
    }
    else {
        std::shared_ptr<ChunkMeshSectionCollection> sectionCollecton = std::make_shared<ChunkMeshSectionCollection>(xz);
        sectionCollecton -> sections[sectionIndex] -> aabb = aabb;
        sectionCollecton -> sections[sectionIndex] -> offsets.insert(sectionCollecton -> sections[sectionIndex] -> offsets.end(), offsetsVector.begin(), offsetsVector.end());
        std::pair<std::string, std::shared_ptr<ChunkMeshSectionCollection>> pair(key, sectionCollecton);
        offsetsMap.insert(pair);
    }
}


void InstanceMeshDrawable::instanceDraw(std::shared_ptr<Camera> camera, std::shared_ptr<FrameBuffer> frameBuffer) {
    
    std::lock_guard<std::mutex> lock(this -> mutex);
    // step.1 buffer vao
    if (vaoDataBuffered == false) {
        bufferVaoData();
    }
    
    // step.2 buffer instance vbo
    std::vector<glm::vec3> combineOffsets;
    for (auto& one : this -> offsetsMap) {
        auto sections = one.second;
        for (auto& two : sections -> sections) {
            // step.3 is aabb box in view frustum ??
            if (camera -> getFrustum().isBoxInFrustum(two -> aabb) == false) {
                continue;
            }
            std::vector<glm::vec3> vec = two -> offsets;
            if (vec.size() != 0) {
                combineOffsets.insert(combineOffsets.end(),vec.begin(), vec.end());
            }
        }
    }
    
    // step.4 buffer subdata if needed
    if (combineOffsets.size() != combineOffsetsSize) {
        combineOffsetsSize = (int)combineOffsets.size();
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, combineOffsets.size() * sizeof(glm::vec3), &combineOffsets[0]);
    }
    
    // step.5 bind texture and draw
    TextureAtlas::sharedInstance().bindTexture();
#if POLYGON_LINE_ENAGLED
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0, combineOffsetsSize);
    glBindVertexArray(0);
#if POLYGON_LINE_ENAGLED
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    TextureAtlas::sharedInstance().unbindTexture();
}

void InstanceMeshDrawable::clearOffsetsFor(VectorXZ&& xz) {
    std::lock_guard<std::mutex> lock(this -> mutex);
    std::string key = getKey(xz);
    bool found = this -> offsetsMap.find(key) != offsetsMap.end();
    if (found) {
        auto sections = this -> offsetsMap.at(key);
        sections -> clear();
    }
}

void InstanceMeshDrawable::clearAll() {
    this -> offsetsMap.clear();
}


std::string InstanceMeshDrawable::getKey(VectorXZ xz) {
    return std::to_string(xz.x) + std::to_string(xz.z);
}
