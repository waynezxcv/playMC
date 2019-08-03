
#ifndef Context_hpp
#define Context_hpp

#include <stdio.h>
#include "Shader.hpp"
#include "NonCopyable.hpp"
#include "FrameBuffer.hpp"


namespace GLL {
    class FramebufferManager : public NonCopyable {
    public:
        
        static FramebufferManager* sharedInstance() {
            static FramebufferManager context;
            return &context;
        };
        
        void presentFrameBufferToScreen(std::shared_ptr<FrameBuffer> frameBuffer);
        
    private:
        
        FramebufferManager() :
        quadShader(Shader("quad.vert","quad.frag")),
        quadVAO(0),
        quadVBO(0),
        quadEBO(0) {
            setupQuad();
        };
        
        void setupQuad();
        Shader quadShader;
        GLuint quadVAO;
        GLuint quadVBO;
        GLuint quadEBO;
    };
}



#endif /* Context_hpp */
