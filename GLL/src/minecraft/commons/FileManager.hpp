

#ifndef FileManager_hpp
#define FileManager_hpp

#include <stdio.h>
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>



namespace GLL {
    class FileManager {
        
    public:
        // 获取工程跟目录
        static std::string getRootDirectory() {
            static char const* envRoot = getenv("PROJECT_ROOT_DIR");
            static std::string root = (envRoot != nullptr ? envRoot : "");
            return root;
        }
        
        // 通过文件路径获取到文件内容
        static std::string getFileContent(const std::string& filePath) {
            std::ifstream inFile(filePath);
            if (!inFile.is_open()) {
                throw std::runtime_error("unable to open file : " + filePath);
            }
            std::stringstream stream;
            stream << inFile.rdbuf();
            return stream.str();
        }
        
        // 获取assets文件夹目录
        static std::string getAssetDirectory() {
            return getRootDirectory() + "assets/";
        }
        
        // 获取shaders文件夹目录
        static std::string getShaderDirectory() {
            return getRootDirectory() + "shaders/";
        }
        
        // 获取assets文件夹下的文件路径
        static std::string getAssetPath(const std::string& fileName) {
            return getRootDirectory() + "assets/" + fileName;
        }
        
        // 获取shaders文件夹下的文件路径
        static std::string getShaderPath(const std::string& fileName) {
            return getRootDirectory() + "shaders/" + fileName;
        }
        
    private:
        FileManager();
    };
}

#endif /* FileManager_hpp */
