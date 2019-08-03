
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "NonCopyable.hpp"
#include "FrameBufferCache.hpp"


namespace GLL {
    
    typedef enum {
        WindowKeyboardEventType_W,
        WindowKeyboardEventType_S,
        WindowKeyboardEventType_A,
        WindowKeyboardEventType_D,
        WindowKeyboardEventType_Q,
        WindowKeyboardEventType_E,
    } WindowKeyboardEventType;
    
    
    class Window : public NonCopyable {
        
    public:
        
        std::function<void(std::shared_ptr<FrameBuffer>)> displayRunloopCallback;
        std::function<void(const int& width, const int& height)> framebufferSizeChangedCallback;
        
        std::function<void(const double &xPosition, const double &yPosition)> mouseMoveEventCallback;
        std::function<void(const double &xPosition, const double &yPosition)> mouseScrollEventCallback;
        std::function<void(const WindowKeyboardEventType &eventType,  float deltaTime)> keyboardTapEventCallback;
        std::function<void(const double &xPosition, const double &yPosition)> mouseLeftButtonDragEventCallback;
        std::function<void(const double &xPosition, const double &yPosition)> mouseRightButtonDragEventCallback;
        
        
        void setBackgroundColor(const glm::vec3 &color);
        void setup();
        void setCurrentFrameBufferSize(const int& x, const int& y);
        glm::vec2 getCurrentFrameBufferSize() const;
        void runloop();
        
        
        
        static Window* sharedInstance() {
            static Window instance;
            return &instance;
        };
        
    private:
        
        Window(){};
        glm::vec3 backgroundColor;
        glm::vec2 currentFrameBufferSize{WINDOW_WIDTH, WINDOW_HEIGHT};
        GLFWwindow *glfwWindow;
        void keyboardEventHandle(GLFWwindow *window);
        
       
    };
    
} // namespace GLL

