//
//  FPSCounter.hpp
//  GLL
//
//  Created by 刘微 on 2019/8/3.
//  Copyright © 2019 liu.wei. All rights reserved.
//

#ifndef FPSCounter_hpp
#define FPSCounter_hpp

#include <stdio.h>
#include <chrono>
#include <iostream>

namespace GLL {
    class FPSCounter {
        
    public:
        FPSCounter();
        ~FPSCounter();
        void update();
    private:
        float fps = 0.0f;
        int frameCount = 0;
        float lastTime;
    };
}

#endif /* FPSCounter_hpp */
