

#include <iostream>
#include "Application.hpp"
#include "MasterRender.hpp"
#include "World.hpp"
#include "Camera.hpp"

int main(int argc, char *argv[]) {
    GLL::CameraConfig config =  GLL::Camera::defaultCameraConfig();
    config.position = glm::vec3{0.0f, CHUNK_SIZE * CHUNK_SIZE / 4, CHUNK_SIZE * 2};
    config.movementSpeed = 50.0f;
    
    GLL::Application<GLL::World> app(config);
    app.start();
    return 0;
}

