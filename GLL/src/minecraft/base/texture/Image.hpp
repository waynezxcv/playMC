
#ifndef Image_hpp
#define Image_hpp



#include <stdio.h>
#include "FileManager.hpp"

/*
 封装了std_image和fileManager的相关接口来读取图片文件
 */

namespace GLL {
    class Image {
        
    public:
        explicit Image(const std::string& fileName, bool isFlipVertically = false);
        Image(const std::string& directory, const std::string& fileName, bool isFlipVertically = false);
        /*
         三五法则:
         1. 需要析构函数的类也需要拷贝构造函数和拷贝赋值函数。
         2. 需要拷贝操作的类也需要赋值操作，反之亦然。
         3. 析构函数不能是删除的
         4. 如果一个类有删除的或不可访问的析构函数，那么其默认和拷贝构造函数会被定义为删除的。
         5. 如果一个类有const或引用成员，则不能使用合成的拷贝赋值操作。
         */
        Image(Image& rhs);
        Image& operator=(Image& rhs);
        ~Image();
        /*
         get
         */
        uint8_t* getImageData() const;
        int getWidth() const;
        int getHeight() const;
        int getComponentsCount() const;
        
    private:
        
        void setup();
        std::string directory;
        std::string filePath;
        uint8_t* imageData;
        int width;
        int height;
        int components;
        
        bool isFlipVertically = false;
    };
}

#endif /* Image_hpp */
