
#ifndef CubeTexture_hpp
#define CubeTexture_hpp

#include <stdio.h>
#include "Image.hpp"
#include "NonCopyable.hpp"
#include <array>

namespace GLL {
    class CubeTexture : public  NonCopyable {
    public:

        explicit CubeTexture(const std::array<std::string, 6>& paths);
        ~CubeTexture();
        
        void bindTexture();
        void unbindTexture();
        
        
    private:
        GLuint textureHandle;
        void loadFromFilePath(const std::array<std::string, 6>& paths);

    };
}

#endif /* CubeTexture_hpp */
