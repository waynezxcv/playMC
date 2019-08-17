

#include "Window.hpp"
#include "Camera.hpp"
#include "Render.hpp"

namespace GLL {
    
    /* ------------------------------------------ */
    
    template <typename T>
    class Application : public NonCopyable {
    public:
        Application();
        ~Application();
        void start() ;
        std::unique_ptr<T> makeAppDelegate();
        
    private:
        std::unique_ptr<T> appDelegate;
        void mouseMoveEventHandle(double xPosition, double yPosition);
        void mouseLeftButtonDragMoveEventHandle(double xPosition, double yPosition);
        void mouseRightButtonDragMoveEventHandle(double xPosition, double yPosition);
        void mouseScrollEventHandle(double xPosition, double yPosition);
        void keyboardTapEventHandle(WindowKeyboardEventType eventType, float deltaTime);
        void framebufferSizeChangedHandle(int width, int height);
        void displayRunloopHandle(std::shared_ptr<FrameBuffer> frameBuffer);
        CameraMovement cameraMovementFromKeyboardEventType(WindowKeyboardEventType eventType);
    };
    
    
    /* ------------------------------------------ */
    template <typename T>
    Application<T>::Application()
    {
    };
    
    
    template <typename T>
    Application<T>::~Application() {
    }
    
    
    template <typename T>
    std::unique_ptr<T> Application<T>::makeAppDelegate() {
        return std::make_unique<T>();
    }
    
    template <typename T>
    void Application<T>::start() {
        /*
         确保appdelegate创建在window setup之后，否则会因OpenGL没有初始化完崩溃
         */
        GLL::Window::sharedInstance()->setup();
        appDelegate = makeAppDelegate();
        
        
        glm::vec3 color(0.3f, 0.5f, 0.5f);
        GLL::Window::sharedInstance()->setBackgroundColor(color);
        
        GLL::Window::sharedInstance()->mouseMoveEventCallback = [this](double xPosition, double yPosition) -> void {
            this->mouseMoveEventHandle(xPosition, yPosition);
        };
        
        GLL::Window::sharedInstance()->mouseLeftButtonDragEventCallback = [this](double xPosition, double yPosition) -> void {
            this -> mouseLeftButtonDragMoveEventHandle(xPosition, yPosition);
        };
        
        GLL::Window::sharedInstance() -> mouseRightButtonDragEventCallback = [this] (double x, double y) -> void {
            this -> mouseRightButtonDragMoveEventHandle(x,y);
        };
        
        GLL::Window::sharedInstance()->mouseScrollEventCallback = [this](double xPosition, double yPosition) -> void {
            this->mouseScrollEventHandle(xPosition, yPosition);
        };
        
        GLL::Window::sharedInstance()->keyboardTapEventCallback = [this](WindowKeyboardEventType eventType, float deltaTime) -> void {
            this->keyboardTapEventHandle(eventType, deltaTime);
        };
        
        GLL::Window::sharedInstance()->displayRunloopCallback = [this](std::shared_ptr<FrameBuffer> framebuffer) -> void {
            this->displayRunloopHandle(framebuffer);
        };
        
        GLL::Window::sharedInstance() -> framebufferSizeChangedCallback = [this](int widht, int height) -> void {
            this -> framebufferSizeChangedHandle(widht, height);
        };
        
        GLL::Window::sharedInstance()->runloop();
    }
    
    
    template <typename T>
    void Application<T>::mouseMoveEventHandle(double xPosition, double yPosition) {
        this -> appDelegate ->camera -> processMouseMovement(xPosition, -yPosition);
    }
    
    
    template <typename T>
    void Application<T>::mouseLeftButtonDragMoveEventHandle(double xPosition, double yPosition) {
        this -> appDelegate->camera -> processMouseLeftButtonDragMovement(xPosition, -yPosition);
    }
    
    
    template <typename T>
    void Application<T>::mouseRightButtonDragMoveEventHandle(double xPosition, double yPosition) {
        
    }
    
    template <typename T>
    void Application<T>::mouseScrollEventHandle(double xPosition, double yPosition) {
        this -> appDelegate->camera -> processMouseScroll(yPosition);
    }
    
    
    template <typename T>
    void Application<T>::keyboardTapEventHandle(WindowKeyboardEventType eventType, float deltaTime) {
        this -> appDelegate->camera -> processKeyboard(cameraMovementFromKeyboardEventType(eventType), deltaTime);
    }
    
    template <typename T>
    void Application<T>::framebufferSizeChangedHandle(int width, int height) {
        FrameBufferCache::sharedInstance() -> clearAllCaches();
        this -> appDelegate->camera -> processFrameBufferSizeChanged(width, height);
    }
    
    template <typename T>
    CameraMovement Application<T>::cameraMovementFromKeyboardEventType(WindowKeyboardEventType eventType) {
        switch (eventType) {
            case WindowKeyboardEventType_W: return CameraMovement_Forward;
            case WindowKeyboardEventType_S: return CameraMovement_Backward;
            case WindowKeyboardEventType_A: return CameraMovement_Left;
            case WindowKeyboardEventType_D: return CameraMovement_Right;
            case WindowKeyboardEventType_Q: return CameraMovement_Up;
            case WindowKeyboardEventType_E: return CameraMovement_Down;
            default: return CameraMovement_Unkonw;
        }
    }
    
    template <typename T>
    void Application<T>::displayRunloopHandle(std::shared_ptr<FrameBuffer> frameBuffer) {
        appDelegate -> draw(frameBuffer);
    }
    
} // namespace GLL


