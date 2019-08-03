

#ifndef Model_hpp
#define Model_hpp

#include <stdio.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.hpp"
#include "Mesh.hpp"


namespace GLL {
    
    typedef struct {
        glm::vec3 rotate;
        float angle;
    } ModelRotation;
    
    
    class Model {
    public:
        explicit Model(const std::string& path, bool instance = false);
        void load();
        void draw(const Shader& shader);
        
        std::vector<Mesh::Texture> getLoadedTextures() const;
        std::vector<Mesh> getMeshes() const;
        void setInstanceModelMatrices(const std::vector<glm::mat4>& matrices);
        
    private:
        
        std::string directory;
        std::string path;
        std::vector<Mesh> meshes;
        
        void load(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Mesh::Texture> loadMaterialTextures(aiMaterial* material, aiTextureType textureType, const std::string& typeName);
        std::vector<Mesh::Texture> loadedTextures;
        
        // for instance
        bool isInstanceModel;
        std::vector<glm::mat4>instaceModelMatrices;
        void processInstanceMatrixBuffer();
    };
}

#endif /* Model_hpp */
