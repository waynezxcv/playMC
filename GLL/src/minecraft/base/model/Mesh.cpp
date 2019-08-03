
#include "Mesh.hpp"
#include <iostream>

using namespace GLL;


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures) :
vertices(vertices),
indices(indices),
textures(textures) {
    setup();
}


void Mesh::setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    glVertexAttribPointer(VertextAttribLocaton_Position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(VertextAttribLocaton_Position);
    
    glVertexAttribPointer(VertextAttribLocaton_Normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(VertextAttribLocaton_Normal);
    
    glVertexAttribPointer(VertextAttribLocaton_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(VertextAttribLocaton_TexCoords);
    
    glVertexAttribPointer(VertextAttribLocaton_Tangent, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(VertextAttribLocaton_Tangent);
    
    glVertexAttribPointer(VertextAttribLocaton_Bitangent, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(VertextAttribLocaton_Bitangent);
    
    glBindVertexArray(0);
}


void Mesh::bindModelMatrix(GLuint matrixBufferHandle) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, matrixBufferHandle);
    for (int i = 0; i < 4; i ++) {
        glEnableVertexAttribArray(VertextAttribLocaton_ModelMatrix + i);
        glVertexAttribPointer(VertextAttribLocaton_ModelMatrix + i, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4) , (GLvoid *)(sizeof(glm::vec4) * i) );
        /*
         glVertexAttribDivisor 如果divisor是0， 那么这个数组就是一个常规的顶点属性。
         如果非0，设为为n，那么就会每隔n个实例从这个数组中读取一个新的数据。
         */
        glVertexAttribDivisor(VertextAttribLocaton_ModelMatrix + i, 1);
    }
    glBindVertexArray(0);
}

void Mesh::bindTexture(const Shader& shader) {
    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glUniform1i(glGetUniformLocation(shader.getProgramHandle(), textures[i].samplerName.c_str()), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].textureHandle);
    }
}

void Mesh::draw(const Shader& shader) {
    this -> bindTexture(shader);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::instanceDraw(const Shader& shader, const int& instnaceCount) {
    this -> bindTexture(shader);
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0, instnaceCount);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}
