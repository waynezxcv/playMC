

#ifndef Shader_hpp
#define Shader_hpp

#include <iostream>
#include <stdlib.h>
#include <vector>
#include "FileManager.hpp"



namespace GLL {
    
    // 着色器程序抽象
    class Shader {
        
    public:
        
        
        // 使用默认文件路径
        explicit Shader(const std::string& vsp, const std::string& fsp)  :
        directory(FileManager::getShaderDirectory()) ,
        vertextShaderPath(vsp) ,
        fragmentShaderPath(fsp) ,
        programHandle(0),
        compiled(false) {};
        
        
        // 指定文件路径
        Shader(const std::string& dir,const std::string& vsp, const std::string& fsp) :
        directory(dir) ,
        vertextShaderPath(vsp) ,
        fragmentShaderPath(fsp) ,
        programHandle(0),
        compiled(false) {};
        
        
        ~Shader() {
            glDeleteProgram(this -> programHandle);
        }
        
        
        Shader(const Shader& rhs) {
            vertextShaderPath = rhs.vertextShaderPath;
            fragmentShaderPath = rhs.fragmentShaderPath;
            programHandle = rhs.programHandle;
            compiled = rhs.compiled;
        }
        
        
        Shader& operator= (const Shader& rhs) {
            vertextShaderPath = rhs.vertextShaderPath;
            fragmentShaderPath = rhs.fragmentShaderPath;
            programHandle = rhs.programHandle;
            compiled = rhs.compiled;
            return *this;
        }
        
        // 编译着色器程序
        GLint compile();
        
        // 使用着色器程序
        void use();
        
        // 获取着色器句柄
        GLuint getProgramHandle() const;
        
        // 删除着色器程序
        void deleteProgramHandle();
        
        // 获取attribute参数的location
        GLint getAttributeLocation(const std::string& name);
        
        // 获取uniform参数的location
        GLint getUniformLocation(const std::string& name);
        
        // 设置Uniform参数的一些快捷方法
        void setUniformMatrix4(const std::string& name, glm::mat4 value);
        void setUniformVec3(const std::string& name, glm::vec3 value);
        void setUniformInt(const std::string& name, int value);
        void setUniformFloat(const std::string& name, float value);
        
    private:
        std::string directory;
        // 是否已经编译
        bool compiled;
        std::string vertextShaderPath;
        std::string fragmentShaderPath;
        GLuint programHandle;
    };
}

#endif /* Shader_hpp */
