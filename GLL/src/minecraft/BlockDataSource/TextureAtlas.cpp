
#include "TextureAtlas.hpp"


using namespace GLL;

namespace {
    constexpr GLfloat adjustValue = 0.002f;
    constexpr GLuint IMAGE_SIZE = 256;
    constexpr GLuint INDIVIDUAL_TEX_COUNT = 16;
}


TextureAtlas& TextureAtlas::sharedInstance() {
    static TextureAtlas atlas("texture/texturePack.png");
    return atlas;
}

TextureAtlas::TextureAtlas(const std::string& filePath) :
imageSize(IMAGE_SIZE),
individualTextureCout(INDIVIDUAL_TEX_COUNT)
{
    loadFromFilePath(filePath);
}

TextureAtlas::~TextureAtlas() {
    glDeleteTextures(1, &textureHandle);
}

void TextureAtlas::loadFromFilePath(const std::string& filePath) {
    
    Image image(filePath, true);
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
    glBindTexture(GL_TEXTURE_2D, 0);
    
}

/*
 方向:
 左下 - 右下 - 右上 - 左上
 */
std::vector<glm::vec2> TextureAtlas::textureCoordsFromTexutreIndex(const glm::vec2& textureIndex) {
    
    static const float pixelPerIndividual = 1.0f / (GLfloat)individualTextureCout;
    
    std::vector<glm::vec2> results;
    GLfloat xIndex = textureIndex.x;
    GLfloat yIndex = textureIndex.y;
    
    results.push_back(glm::vec2{(xIndex) * pixelPerIndividual + adjustValue , 1.0f - (yIndex + 1) * pixelPerIndividual + adjustValue}); //左下角
    results.push_back(glm::vec2{(xIndex + 1) * pixelPerIndividual - adjustValue, 1.0f - (yIndex + 1) * pixelPerIndividual + adjustValue}); //右下角
    results.push_back(glm::vec2{(xIndex + 1) * pixelPerIndividual - adjustValue, 1.0f - (yIndex) * pixelPerIndividual - adjustValue}); //右上角
    results.push_back(glm::vec2{(xIndex) * pixelPerIndividual + adjustValue, 1.0f - (yIndex) * pixelPerIndividual - adjustValue}); //左上角
    
    return results;
}


void TextureAtlas::bindTexture() {
    glBindTexture(GL_TEXTURE_2D, textureHandle);
}


void TextureAtlas::unbindTexture() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

