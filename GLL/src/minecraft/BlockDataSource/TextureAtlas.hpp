

#ifndef TextureAtlas_hpp
#define TextureAtlas_hpp

#include <stdio.h>
#include <array>
#include <vector>
#include "Image.hpp"
#include "NonCopyable.hpp"
#include "BasicTexture.hpp"


namespace GLL {
    class TextureAtlas : public NonCopyable {
    public:
        static TextureAtlas& sharedInstance();
        
        std::vector<glm::vec2> textureCoordsFromTexutreIndex(const glm::vec2& textureIndex);
        void bindTexture();
        void unbindTexture();
    private:
        explicit TextureAtlas(const std::string& filePath);
        ~TextureAtlas();

        
        GLuint textureHandle;
        GLuint imageSize;
        GLuint individualTextureCout;
        void loadFromFilePath(const std::string& filePath);
    };
}

#endif /* TextureAtlas_hpp */
