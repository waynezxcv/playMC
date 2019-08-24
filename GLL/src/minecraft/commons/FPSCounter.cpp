

#include "FPSCounter.hpp"


using namespace GLL;

FPSCounter::FPSCounter() {
}

FPSCounter::~FPSCounter() {
}

void FPSCounter::update() {
    this -> frameCount ++;
    float current = (float)glfwGetTime();
    float delta = current - this -> lastTime;
    if ( delta >= 1.0f) {
        this -> fps = frameCount / delta;
        frameCount = 0;
        this -> lastTime = current;
        std::cout<<"[fps] : " <<this -> fps <<std::endl;
    }
}


