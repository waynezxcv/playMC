//
//  FPSCounter.cpp
//  GLL
//
//  Created by 刘微 on 2019/8/3.
//  Copyright © 2019 liu.wei. All rights reserved.
//

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
    if ( delta >= 0.5f) {
        this -> fps = frameCount / delta;
        frameCount = 0;
        this -> lastTime = current;
        std::cout<<"[fps] : " <<this -> fps <<std::endl;
    }
}


