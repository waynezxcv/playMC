
#include "Window.hpp"


float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool isMouseLeftButtonPressing = false;
bool isMouseRightButtonPressing = false;


static inline void frameBufferSizeHandle(GLFWwindow *glfwWindow, int width, int height);
static inline void mouseMoveEventHandle(GLFWwindow *glfwWindow, double xPosition, double yPosition);
static inline void mouseScrollEventHandle(GLFWwindow *glfwWindow, double xoffset, double yoffset);
static inline void mouseButtonEventHandle(GLFWwindow* glfWindow, int a, int b, int c);


using namespace GLL;


void Window::setBackgroundColor(const glm::vec3 &color) {
    backgroundColor = color;
}

void Window::setup() {
    
    if (!glfwInit()) {
        throw "glfw init error";
    }
    
    // 设置opengl版本,重要~
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow *window = nullptr;
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Harvey's Minecraft", NULL, NULL);
    
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("glfw window create error");
    }
    
    glfwMakeContextCurrent(window);
    
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    this -> setCurrentFrameBufferSize(width, height);
    
    // 手动掉一次
    if (this -> framebufferSizeChangedCallback) {
        this -> framebufferSizeChangedCallback(width, height);
    }
    
    
    this -> glfwWindow = window;
    // 屏幕大小切换
    glfwSetFramebufferSizeCallback(window, frameBufferSizeHandle);
    
    // 鼠标光标移动事件
    glfwSetCursorPosCallback(window, mouseMoveEventHandle);
    
    // 鼠标滚轮事件
    glfwSetScrollCallback(window, mouseScrollEventHandle);
    
    // 鼠标按键事件
    glfwSetMouseButtonCallback(window, mouseButtonEventHandle);
}



void Window::setCurrentFrameBufferSize(const int& x, const int& y) {
    this -> currentFrameBufferSize = glm::vec2{x, y};
}


glm::vec2 Window::getCurrentFrameBufferSize() const {
    return this -> currentFrameBufferSize;
}


void Window::runloop() {
    
    while (!glfwWindowShouldClose(this->glfwWindow)) {
        
        
        TextureParameterOptions options = FrameBuffer::getDefaultTextureOptions();
        std::shared_ptr<FrameBuffer> inFlightFrameBuffer = FrameBufferCache::sharedInstance() -> fetchFrameBuffer(currentFrameBufferSize, options);
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        this->keyboardEventHandle(this->glfwWindow);
        
        // 清屏
        glClearColor(this->backgroundColor.x, this->backgroundColor.y, this->backgroundColor.z, 1.0f);
        
        // 渲染
        inFlightFrameBuffer  -> lock();
        if (this->displayRunloopCallback) {
            this->displayRunloopCallback(inFlightFrameBuffer);
        }
        
        glfwSwapBuffers(this->glfwWindow);
        glfwPollEvents();
        inFlightFrameBuffer  -> unlock();
                
    }
    glfwTerminate();
}


void Window::keyboardEventHandle(GLFWwindow *window) {
    // 按ESC
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    // 按W
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (this->keyboardTapEventCallback) {
            this->keyboardTapEventCallback(WindowKeyboardEventType_W, deltaTime);
        }
    }
    
    // 按S
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (this->keyboardTapEventCallback) {
            this->keyboardTapEventCallback(WindowKeyboardEventType_S, deltaTime);
        }
    }
    
    // 按A
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (this->keyboardTapEventCallback) {
            this->keyboardTapEventCallback(WindowKeyboardEventType_A, deltaTime);
        }
    }
    
    // 按D
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (this->keyboardTapEventCallback) {
            this->keyboardTapEventCallback(WindowKeyboardEventType_D, deltaTime);
        }
    }
    
    // Q
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (this->keyboardTapEventCallback) {
            this->keyboardTapEventCallback(WindowKeyboardEventType_Q, deltaTime);
        }
    }
    
    // E
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (this->keyboardTapEventCallback) {
            this->keyboardTapEventCallback(WindowKeyboardEventType_E, deltaTime);
        }
    }
}


static inline void frameBufferSizeHandle(GLFWwindow *glfwWindow, int width, int height) {
    GLL::Window* window = GLL::Window::sharedInstance();
    window -> setCurrentFrameBufferSize(width, height);
    if (window -> framebufferSizeChangedCallback) {
        window -> framebufferSizeChangedCallback(width, height);
    }
    glViewport(0, 0, width, height);
}


static inline void mouseScrollEventHandle(GLFWwindow *glfwWindow, double xoffset, double yoffset) {
    GLL::Window *window = GLL::Window::sharedInstance();
    if (window->mouseScrollEventCallback) {
        window->mouseScrollEventCallback(xoffset, yoffset);
    }
}

static inline void mouseButtonEventHandle(GLFWwindow* glfWindow, int buttonType, int action , int mods) {
    
    if (buttonType == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        isMouseLeftButtonPressing = true;
    }
    else if (buttonType == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        isMouseLeftButtonPressing = false;
    }
    else if (buttonType == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        isMouseRightButtonPressing = true;
    }
    else if (buttonType == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        isMouseRightButtonPressing = false;
    }
}

static inline void mouseMoveEventHandle(GLFWwindow *glfwWindow, double xPosition, double yPosition) {
    
    // 记录上次鼠标操作
    if (firstMouse) {
        lastX = xPosition;
        lastY = yPosition;
        firstMouse = false;
    }
    
    // 计算鼠标xy偏移值
    float xoffset = xPosition - lastX;
    float yoffset = yPosition - lastY;
    lastX = xPosition;
    lastY = yPosition;
    
    GLL::Window *window = GLL::Window::sharedInstance();
    
    if (isMouseLeftButtonPressing) {
        if (window -> mouseLeftButtonDragEventCallback) {
            window -> mouseLeftButtonDragEventCallback(xoffset, yoffset);
        }
    }
    
    else if (isMouseRightButtonPressing) {
        if (window -> mouseRightButtonDragEventCallback) {
            window -> mouseRightButtonDragEventCallback(xoffset, yoffset);
        }
    }
    
    else {
        if (window->mouseMoveEventCallback) {
            window->mouseMoveEventCallback(xoffset, yoffset);
        }
    }
}
