
#ifndef Mesh_hpp
#define Mesh_hpp

#include <stdio.h>
#include <vector>
#include "Shader.hpp"




/*
 网格(Mesh)代表的是单个的可绘制实体
 */

namespace GLL {
    
    
    
    class Mesh {
    public:
        
        // 纹理贴图类型
        enum TextureType {
            TextureType_None,
            TextureType_Diffuse, // 漫反射
            TextureType_Specular, // 镜面反射
            TextureType_Normal, // 法线
            TextureType_Ambient ,// 环境光
        };
        
        
        struct Vertex {
            // 位置
            glm::vec3 position;
            // 顶点法线
            glm::vec3 normal;
            // 纹理坐标
            glm::vec2 texCoords;
            // 切线
            glm::vec3 tangent;
            // 双切线
            glm::vec3 bitangent;
        };
        
        
        struct Texture {
            // 纹理的句柄
            GLuint textureHandle;
            // 贴图类型
            TextureType texutreType;
            // 纹理采样器在shader中的命名
            std::string samplerName;
            // 路径
            std::string path;
        };
        
        enum VertextAttribLocaton {
            VertextAttribLocaton_Position = 0,
            VertextAttribLocaton_Normal = 1,
            VertextAttribLocaton_TexCoords = 2,
            VertextAttribLocaton_Tangent = 3,
            VertextAttribLocaton_Bitangent = 4,
            VertextAttribLocaton_ModelMatrix = 5,
        };
        
    public:
        // 顶点坐标
        std::vector<Vertex> vertices;
        // 顶点序号
        std::vector<GLuint> indices;
        // 纹理
        std::vector<Texture> textures;
        
    public:
        Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
        void draw(const Shader& shader);
        void instanceDraw(const Shader& shader, const int& instnaceCount);
        void bindModelMatrix(GLuint matrixBufferHandle);
    private:
        GLuint VAO, VBO, EBO = 0;
        void setup();
        void bindTexture(const Shader& shader);
    };
}

#endif /* Mesh_hpp */
