

#ifndef ModelRender_hpp
#define ModelRender_hpp

#include <stdio.h>
#include "Render.hpp"
#include "Shader.hpp"
#include "Model.hpp"

/*
 用于渲染obj等格式的模式
 */

namespace GLL {
    class ModelRender : public Render {
    public:
        
        ModelRender(const std::string& modelPath,  glm::vec3 position);
        ~ModelRender();
        
        void renderInit() override;
        void draw(Camera* camera,  std::shared_ptr<FrameBuffer> frameBuffer) override;
        
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
        
    private:
        Shader shader;
        Model model;
        glm::vec3 position;
    };
}

#endif /* ModelRender_hpp */


