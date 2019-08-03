
#include "CubeTexture.hpp"


using namespace GLL;


CubeTexture::CubeTexture(const std::array<std::string, 6>& paths) :
textureHandle(0)
{
    loadFromFilePath(paths);
}


CubeTexture::~CubeTexture() {
    glDeleteTextures(1, &textureHandle);
}

void CubeTexture::bindTexture() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);
}


void CubeTexture::unbindTexture() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


void CubeTexture::loadFromFilePath(const std::array<std::string, 6>& paths) {
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);
    
    for (int i = 0; i < 6; i ++) {
        std::string path = paths[i];
        Image image(path);
        // 图片有几个颜色通道
        GLenum format = 0;
        if (image.getComponentsCount() == 1) {
            format = GL_RED; // 若只有1个，按红色处理
        }
        else if (image.getComponentsCount()  == 3) {
            format = GL_RGB;
        }
        else if (image.getComponentsCount()  == 4) {
            format = GL_RGBA;
        }
        
        auto param = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
        int width = image.getWidth();
        int height = image.getHeight();
        
        glTexImage2D(param,
                     0,
                     format,
                     width,
                     height,
                     0,
                     format,
                     GL_UNSIGNED_BYTE,
                     image.getImageData());
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
