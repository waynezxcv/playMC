

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
