
#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

using namespace GLL;




Image::Image(const std::string& fileName, bool isFlipVertically) :
directory(FileManager::getAssetDirectory()),
filePath(directory + fileName),
imageData(nullptr),
width(0),
height(0),
components(0) ,
isFlipVertically(isFlipVertically) {
    setup();
}



Image::Image(const std::string& directory, const std::string& fileName, bool isFlipVertically) :
directory(directory),
filePath(directory + fileName),
imageData(nullptr),
width(0),
height(0),
components(0),
isFlipVertically(isFlipVertically) {
    setup();
}


Image::Image(Image& rhs) {
    this -> directory = rhs.directory;
    this -> filePath = rhs.filePath;
    setup();
}


Image& Image::operator=(Image& rhs) {
    this -> directory = rhs.directory;
    this -> filePath = rhs.filePath;
    setup();
    return *this;
}


Image::~Image() {
    stbi_image_free(this -> imageData);
}

void Image::setup() {
    int width, height, nrChannel;
    stbi_set_flip_vertically_on_load(isFlipVertically);
    this -> imageData = stbi_load(this -> filePath.c_str(), &width, &height, &nrChannel, 0);
    this -> width = width;
    this -> height = height;
    this -> components = nrChannel;
    if (this -> imageData == nullptr) {
        std::cerr<<"error : Image setup failed at path :"<<this -> filePath <<std::endl;
    }
}


/*
 get
 */
uint8_t* Image::getImageData() const {
    return this -> imageData;
}

int Image::getWidth() const {
    return this -> width;
}

int Image::getHeight() const {
    return this -> height;
}

int Image::getComponentsCount() const {
    return this -> components;
};
