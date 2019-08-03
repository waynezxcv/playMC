

#include "InstanceRender.hpp"


using namespace GLL;


InstanceRender::InstanceRender(const std::string& modelPath) :
model( Model(FileManager::getAssetPath(modelPath), true)),
shader( Shader("instanceModelLoad.vert","instanceModelLoad.frag") )
{
}


InstanceRender::~InstanceRender() {
    shader.deleteProgramHandle();
}

void InstanceRender::renderInit()  {
    
    // default config
    glm::vec3 scale = glm::vec3{1.0f,1.0f,1.0f};
    ModelRotation rotation;
    rotation.rotate = glm::vec3{0.0f, 0.0f, 0.0f};
    rotation.angle = 1.0f;
    this -> scale = scale;
    this -> rotation = rotation;
    
    // 光源位置
    this -> lightPosition = glm::vec3{0,0.5f,-0.2f};
    
    // 光的方向
    this -> lightDirection = glm::vec3{0.0f, 0.0f, 0.0f};
    
    // 光源的圈大小
    this -> lightCutOff = glm::cos(glm::radians(12.5f * 5));
    this -> lightOuterCutOff = glm::cos(glm::radians(17.5f * 5));
    
    // 环境光
    this -> lightAmbient =  glm::vec3{1.0f, 1.0f, 1.0f};
    this -> lightDiffuse = glm::vec3{3.0f, 3.0f, 3.0f};
    this -> lightSpecular = glm::vec3{5.0f, 5.0f, 5.0f};
    
    this -> lightConstant = 2.0f;
    this -> lightLinear = 0.1f;
    this -> lightQuadratic = 0.032f;
    this -> materialShininess = 32.0f;
    shader.compile();
    
    model.setInstanceModelMatrices(this -> getModelMatrices());
    model.load();
}


void InstanceRender::draw(Camera* camera,  std::shared_ptr<FrameBuffer> frameBuffer) {
    glEnable(GL_DEPTH_TEST);
    shader.use();
    shader.setUniformVec3("cameraPosition", camera -> getCameraPosition());
    shader.setUniformVec3("light.position", camera -> getCameraPosition());
    shader.setUniformVec3("light.direction",camera -> getCameraFoward());
    shader.setUniformFloat("light.cutOff", lightCutOff);
    shader.setUniformFloat("light.outerCutOff", lightOuterCutOff);
    shader.setUniformVec3("light.ambient", lightAmbient);
    shader.setUniformVec3("light.diffuse",  lightDiffuse);
    shader.setUniformVec3("light.specular", lightSpecular);
    shader.setUniformFloat("light.constant", lightConstant);
    shader.setUniformFloat("light.linear", lightLinear);
    shader.setUniformFloat("light.quadratic", lightQuadratic);
    shader.setUniformFloat("material.shininess", materialShininess);
    shader.setUniformMatrix4("projection", camera -> getProjectionMatrix());
    shader.setUniformMatrix4("view", camera -> getViewMatrix());
    this -> model.draw(shader);
}

