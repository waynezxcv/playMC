
#ifndef InstanceRender_hpp
#define InstanceRender_hpp

#include <stdio.h>
#include "Render.hpp"
#include "Shader.hpp"
#include "Model.hpp"



namespace GLL {
    
    class InstanceRender : public Render {
        
    public:
        std::function<std::vector<glm::mat4>(void)> getModelMatrices;
        
        glm::vec3 lightPosition;
        glm::vec3 lightDirection;
        float lightCutOff;
        float lightOuterCutOff;
        glm::vec3 lightAmbient;
        glm::vec3 lightDiffuse;
        glm::vec3 lightSpecular;
        float lightConstant;
        float lightLinear;
        float lightQuadratic;
        float materialShininess;
        glm::vec3 scale;
        ModelRotation rotation;
        
    public:
        InstanceRender(const std::string& modelPath);
        
        ~InstanceRender();
        void renderInit() override;
        void draw(std::shared_ptr<Camera> camera,  std::shared_ptr<FrameBuffer> frameBuffer) override;
    private:
        Shader shader;
        Model model;
    };
}


#endif /* InstanceRender_hpp */
