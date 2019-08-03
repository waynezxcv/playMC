
#include "BasicTexture.hpp"


using namespace GLL;

BasicTexture::BasicTexture(const std::string& filePath) :
textureHandle(0)
{
    loadFromFilePath(filePath);
}


void BasicTexture::loadFromFilePath(const std::string& filePath) {
    Image image(filePath);
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
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glTexImage2D(GL_TEXTURE_2D,0,format,image.getWidth(),image.getHeight(),0,format,GL_UNSIGNED_BYTE,image.getImageData());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

BasicTexture::~BasicTexture() {
    glDeleteTextures(1, &textureHandle);
}

void BasicTexture::bindTexture() {
    glBindTexture(GL_TEXTURE_2D, textureHandle);
}

void BasicTexture::unbindTexture() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

