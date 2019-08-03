
#ifndef Cube_hpp
#define Cube_hpp

#include <stdio.h>
#include "Shader.hpp"
#include "Image.hpp"



namespace GLL {
    class Cube {
    public:
        
        Cube(const std::string& imageName, bool instance = false);
        ~Cube();
        
        void setup();
        void draw(const Shader& shader);

    private:
        GLuint VAO;
        GLuint VBO;
        GLuint textrueHandle;
        
        bool isInstance;
        bool instanceModelMatrix;
    };
}

#endif /* Cube_hpp */
