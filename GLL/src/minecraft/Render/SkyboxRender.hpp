
#ifndef SkyboxRender_hpp
#define SkyboxRender_hpp

#include <stdio.h>
#include "Render.hpp"
#include "Shader.hpp"

/*
 用于渲染天空盒
 */
namespace GLL {
    
    class SkyboxRender : public Render {
        
        public :
        
        SkyboxRender();
        SkyboxRender(std::vector<std::string> faceImageNames);
        ~SkyboxRender();
        
        void renderInit() override;
        void draw(Camera* camera, std::shared_ptr<FrameBuffer> frameBuffer) override;
        
    private:
        GLuint VBO;
        GLuint VAO;
        GLuint texutureHandle;
        Shader shader;
        std::vector<std::string> faceImageNames;
    };
}

#endif /* SkyboxRender_hpp */
