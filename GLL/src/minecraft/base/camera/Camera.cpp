
#include "Camera.hpp"
#include "MatrixMaker.hpp"
#include "Window.hpp"


using namespace GLL;

Camera::Camera(const CameraConfig& config) :
cameraPosition(config.position),
cameraUp(config.up),
cameraForward(config.forward),
pitch(config.pitch),
yaw(config.yaw),
fovZoomAngle(config.fovZoomAngle),
movementSpeed(config.movementSpeed),
mouseSensitivity(config.mouseSensitivity),
nearClippingDistance(config.nearClippingDistance),
farClippingDistance(config.farClippingDistance),
frustumFitPadding(config.frustumFitPadding),
worldUp(config.up)
{
    update();
};

Camera::~Camera(){};

const glm::mat4& Camera::getViewMatrix() const noexcept {
    return viewMatrix;
}

const glm::mat4& Camera::getProjectionMatrix() const noexcept {
    return projectionMatrix;
}


const glm::vec3& Camera::getCameraPosition() const noexcept {
    return cameraPosition;
}

const glm::vec3& Camera::getCameraFoward() const noexcept {
    return cameraForward;
}


void Camera::processKeyboard(CameraMovement movement, float deltaTime) {
    
    float velocity = movementSpeed * deltaTime;
    
    switch (movement) {
        case CameraMovement_Forward: {
            cameraPosition += cameraForward * velocity;
        }
            break;
            
        case CameraMovement_Backward: {
            cameraPosition -= cameraForward * velocity;
        }
            break;
            
        case CameraMovement_Left: {
            cameraPosition -= cameraRight * velocity;
        }
            break;
            
        case CameraMovement_Right: {
            cameraPosition += cameraRight * velocity;
        }
            break;
            
        case CameraMovement_Up: {
            cameraPosition += cameraUp * velocity;
        }
            break;
        case CameraMovement_Down: {
            cameraPosition -= cameraUp * velocity;
        };
            break;
            
        case CameraMovement_Unkonw: {
            
        }
            break;
    }
    update();
}


void Camera::processMouseMovement(double x, double y) {
    // do nothing...
}


void Camera::processMouseLeftButtonDragMovement(double x, double y) {
    
    x *= mouseSensitivity;
    y *= mouseSensitivity;
    
    yaw += x;
    pitch += y;
    
    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }
    
    update();
}

void Camera::processFrameBufferSizeChanged(int width, int height) {
    if (height == 0 || width == 0) {
        return;
    }
    this -> windowAspect = (GLfloat)width / (GLfloat)height;
    update();
}

void Camera::processMouseScroll(double value) {
    if (fovZoomAngle >= 1.0f && fovZoomAngle <= 45.0f) {
        fovZoomAngle -= value;
    }
    if (fovZoomAngle <= 1.0f) {
        fovZoomAngle = 1.0f;
    }
    if (fovZoomAngle >= 45.0f) {
        fovZoomAngle = 45.0f;
    }
    update();
}

void Camera::update() {
    
    glm::vec3 forwad;
    forwad.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    forwad.y = sin(glm::radians(pitch));
    forwad.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    
    this -> cameraForward = glm::normalize(forwad);
    this -> cameraRight = glm::normalize(glm::cross(this -> cameraForward, this -> worldUp));
    this -> cameraUp = glm::normalize(glm::cross(this -> cameraRight, this -> cameraForward));
    
    this -> viewMatrix = MatrixMaker::makeViewMatrix(this -> cameraPosition, this -> cameraPosition + this -> cameraForward, this -> cameraUp);
    this -> projectionMatrix = MatrixMaker::makeProjectionMatrix(this -> windowAspect,
                                                                 this -> fovZoomAngle,
                                                                 this -> nearClippingDistance,
                                                                 this -> farClippingDistance);
    this -> frustum.update(projectionMatrix * viewMatrix);
    
    
    if (this -> cameraDidUpdated) {
        this -> cameraDidUpdated();
    }
}

const ViewFrustum& Camera::getFrustum() const noexcept {
    return this -> frustum;
}
