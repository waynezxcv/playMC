
#ifndef BasicTexture_hpp
#define BasicTexture_hpp

#include <stdio.h>
#include "Image.hpp"
#include "NonCopyable.hpp"



namespace GLL {
    class BasicTexture : public  NonCopyable {
    public:
        explicit BasicTexture(const std::string& filePath);
        ~BasicTexture();
        
        void bindTexture();
        void unbindTexture();
        
    private:
        GLuint textureHandle;
        void loadFromFilePath(const std::string& filePath);
    };
}

#endif /* BasicTexture_hpp */
