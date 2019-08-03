
#include "Model.hpp"
#include "Image.hpp"

using namespace GLL;

Model::Model(const std::string& path, bool instance) :
path(path),
isInstanceModel(instance)
{
};


std::vector<Mesh::Texture> Model::getLoadedTextures() const {
    return this -> loadedTextures;
}

std::vector<Mesh> Model::getMeshes() const {
    return this -> meshes;
}


// 读取指定路径下的纹理，并生成纹理
static GLuint texutreFromFile(const char* name, std::string directory) {
    
    directory = directory + "/";
    std::string fileName = std::string(name);
    
    GLuint textureHandle;
    glGenTextures(1, &textureHandle);
    
    Image image(directory, fileName);
    if (image.getImageData() == nullptr) {
        std::cerr<<"error : texture load file failed at path :"<<fileName<<std::endl;
        return textureHandle;
    }
    
    // 图片有几个颜色通道
    GLenum format = 0;
    if (image.getComponentsCount() == 1) {
        format = GL_RED; // 若只有1个，按红色处理
    }
    else if (image.getComponentsCount()  == 3) {
        format = GL_RGB;
    }
    else if (image.getComponentsCount()  == 4) {
        format = GL_RGBA;
    }
    
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, format, image.getWidth(), image.getHeight(), 0, format, GL_UNSIGNED_BYTE, image.getImageData());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return textureHandle;
}


void Model::draw(const Shader& shader) {
    for (int i = 0; i < meshes.size(); i ++) {
        Mesh mesh = meshes[i];
        if (isInstanceModel == true) {
            mesh.instanceDraw(shader, (int)instaceModelMatrices.size());
        }
        else {
            mesh.draw(shader);
        }
    }
}


void Model::setInstanceModelMatrices(const std::vector<glm::mat4>& matrices) {
    this -> instaceModelMatrices = matrices;
}


/*
 Importer读取文件，加载处scene
 再遍历scene的所有node,每个node包含若干的mesh
 */
void Model::load() {
    if (isInstanceModel == true && instaceModelMatrices.size() == 0) {
        std::cerr<<"error :"<<"the instance model matrices is empty .."<<std::endl;
        return;
    }
    
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr<<"error :"<<importer.GetErrorString()<<std::endl;
        return;
    }
    
    directory = path.substr(0, path.find_last_of("/"));
    processNode(scene -> mRootNode, scene);
    
    if (isInstanceModel) {
        this -> processInstanceMatrixBuffer();
    }
}


void Model::processInstanceMatrixBuffer() {
    GLuint modelMatrixBufferHandle;
    glGenBuffers(1, &modelMatrixBufferHandle);
    glBindBuffer(GL_ARRAY_BUFFER, modelMatrixBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instaceModelMatrices.size(), &instaceModelMatrices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    /*
     为模型中的每一个mesh 传递 model matrix
     */
    for (unsigned int i = 0; i < this -> meshes.size(); i ++) {
        Mesh mesh = meshes[i];
        mesh.bindModelMatrix(modelMatrixBufferHandle);
    }
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // 将node上的mesh封装成我们自己的类，再加入数组中
    for (int i = 0; i < node -> mNumMeshes; i ++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // 递归遍历子node
    for (int i = 0; i < node -> mNumChildren; i ++) {
        processNode(node -> mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    /*
     顶点信息
     */
    std::vector<Mesh::Vertex> vertices;
    
    for (int i = 0; i < mesh -> mNumVertices; i++ ) {
        Mesh::Vertex vertex;
        // 位置顶点信息
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        
        // 顶点法线
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;
        
        // 纹理坐标
        if (mesh -> mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
        }
        // 如果不包含纹理坐标
        else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
        
        // 切线
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.tangent = vector;
        
        // 双切线
        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.bitangent = vector;
        
        vertices.push_back(vertex);
    }
    
    /*
     顶点序号
     */
    std::vector<unsigned int> indices;
    for (int i = 0; i < mesh->mNumFaces; i++ ) {
        aiFace face = mesh -> mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    
    /*
     纹理
     */
    std::vector<Mesh::Texture> textures;
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    
    // 1. 漫反射
    std::vector<Mesh::Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "material.diffuseSampler");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    
    // 2. 镜面反射
    std::vector<Mesh::Texture> specularMaps =  loadMaterialTextures(material, aiTextureType_SPECULAR, "material.specularSampler");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    
    // 3. 法线
    std::vector<Mesh::Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "material.normalSampler");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    
    // 4. 环境光
    std::vector<Mesh::Texture> ambientMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "material.ambientSampler");
    textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());
    
    return Mesh(vertices, indices, textures);
}

std::vector<Mesh::Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName ) {
    
    std::vector<Mesh::Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        
        aiString str;
        mat->GetTexture(type, i, &str);
        
        // 检查是否有缓存
        bool skip = false;
        for (unsigned int j = 0; j < loadedTextures.size(); j++) {
            if(std::strcmp(loadedTextures[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(loadedTextures[j]);
                skip = true;
                break;
            }
        }
        
        if (!skip) {
            Mesh::Texture texture;
            texture.textureHandle = texutreFromFile(str.C_Str(), this->directory);
            texture.samplerName = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            loadedTextures.push_back(texture);
        }
    }
    return textures;
}




