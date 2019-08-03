


#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <OpenGL/OpenGL.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "Camera.hpp"
#include "FramebufferManager.hpp"
#include "FrameBuffer.hpp"


#ifndef Render_hpp
#define Render_hpp

/*
 render抽象类
 */
namespace GLL {
    class Render {
    public:
        virtual ~Render(){};
        virtual void renderInit(){};
        virtual void willDraw(Camera* camera, std::shared_ptr<FrameBuffer> frameBuffer) {};
        virtual void draw(Camera *camera,  std::shared_ptr<FrameBuffer> frameBuffer){};
        virtual void didDraw(Camera* camera,  std::shared_ptr<FrameBuffer> frameBuffer) {};
    };
} // namespace GLL


#endif
