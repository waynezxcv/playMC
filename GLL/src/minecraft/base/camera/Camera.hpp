

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include <vector>
#include <mutex>
#include <thread>

#include "Frustum.hpp"

namespace GLL {
    
    typedef struct  {
        glm::vec3 position;
        glm::vec3 up;
        glm::vec3 forward;
        float pitch;
        float yaw;
        float fovZoomAngle;
        float movementSpeed;
        float mouseSensitivity;
        float nearClippingDistance;
        float farClippingDistance;
        float frustumFitPadding;
    } CameraConfig;
    
    
    typedef enum {
        CameraMovement_Forward,
        CameraMovement_Backward,
        CameraMovement_Left,
        CameraMovement_Right,
        CameraMovement_Up,
        CameraMovement_Down,
        CameraMovement_Unkonw,
    } CameraMovement;
    
    class Camera {
        
    public:
        
        static CameraConfig defaultCameraConfig() {
            CameraConfig config;
            config.position = glm::vec3{0.0f, 0.0f, 3.5f};
            config.up = glm::vec3{0.0f, 1.0f, 0.0f};
            config.forward = glm::vec3{0.0f, 0.0f, -1.0f};
            config.pitch = 0.0f;
            config.yaw = -90.0f;
            config.fovZoomAngle = 90.0f;
            config.movementSpeed = 5.0;
            config.mouseSensitivity = 0.1f;
            config.nearClippingDistance = 1.0f;
            config.farClippingDistance = 1000.0f;
            config.frustumFitPadding = 0.02f;
            return config;
        };
        
        
        const glm::mat4& getViewMatrix() const noexcept;
        const glm::mat4& getProjectionMatrix() const noexcept;
        const glm::vec3& getCameraPosition() const noexcept;
        const glm::vec3& getCameraFoward() const noexcept;
        const ViewFrustum& getFrustum() const noexcept;
        
        Camera(const CameraConfig& config = Camera::defaultCameraConfig());
        ~Camera();
        
        void processKeyboard(CameraMovement movement, float deltaTime);
        void processMouseScroll(double value);
        void processMouseMovement(double x, double y);
        void processMouseLeftButtonDragMovement(double x, double y);
        void processFrameBufferSizeChanged(int width, int height);
        
        std::function<void()> cameraDidUpdated;
        
    private:
        
        void update();
        // 摄像机相关信息
        glm::vec3 cameraPosition;
        glm::vec3 cameraUp;
        glm::vec3 cameraForward;
        glm::vec3 cameraRight;
        
        GLfloat pitch;
        GLfloat roll;
        GLfloat yaw;
        
        //世界坐标的up轴
        glm::vec3 worldUp;
        // 投影矩阵
        glm::mat4 projectionMatrix;
        // 观察矩阵
        glm::mat4 viewMatrix;
        
        // 屏幕宽高比
        GLfloat windowAspect = WINDOW_WIDTH / WINDOW_HEIGHT;
        
        GLfloat nearClippingDistance;
        
        GLfloat farClippingDistance;
        
        GLfloat frustumFitPadding;
        
        // 移动速度
        GLfloat movementSpeed;
        // pitch 、yaw轴调整 鼠标灵敏度
        GLfloat mouseSensitivity;
        // fov角度
        GLfloat fovZoomAngle;
        
        // 视线平截头体
        ViewFrustum frustum;
        
        std::mutex cameraMutex;
    };
}


#endif /* Camera_hpp */
