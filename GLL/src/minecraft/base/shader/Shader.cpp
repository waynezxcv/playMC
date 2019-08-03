
#include "Shader.hpp"
#include <fstream>
#include <sstream>


using namespace GLL;


static void readFileToString(std::string filePath, std::string& string) {
    //创建一个文件流对象，并打开文件
    std::ifstream fin(filePath);
    
    //创建字符串流对象
    std::ostringstream sin;
    
    //把文件流中的字符输入到字符串流中
    sin <<fin.rdbuf();
    
    //获取字符串流中的字符串
    string = sin.str();
    fin.close();
    fin.clear();
}

GLint Shader::compile() {
    // 根据文件路径读取着色器程序源码
    std::string vertextShaderString;
    std::string finalPath1 = this -> directory + this -> vertextShaderPath;
    readFileToString(finalPath1, vertextShaderString);
    
    std::string fragmentShaderString;
    std::string finalPath2 = this -> directory + this -> fragmentShaderPath;
    readFileToString(finalPath2, fragmentShaderString);
    
    const GLchar* vertextShaderSource = vertextShaderString.c_str();
    const GLchar* fragmentShaderSource = fragmentShaderString.c_str();
    
    GLuint vertextShader, fragmentShader;
    GLchar message [512];
    GLint result;
    
    // 编译顶点着色器
    vertextShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertextShader, 1, &vertextShaderSource, NULL);
    glCompileShader(vertextShader);
    glGetShaderiv(vertextShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vertextShader, 512, NULL, message);
        std::cerr<<"error : vertext shader compile failed :"<<message<<std::endl;
        return result;
    }
    
    // 编译片段着色器
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, message);
        std::cerr<<"error : fragment shader compile failed :"<<message<<std::endl;
        return result;
    }
    
    // 创建着色器程序
    this -> programHandle = glCreateProgram();
    
    // 装载
    glAttachShader(this -> programHandle , vertextShader);
    glAttachShader(this -> programHandle , fragmentShader);
    
    // 链接
    glLinkProgram(this -> programHandle);
    glGetProgramiv(this -> programHandle , GL_LINK_STATUS, &result);
    
    if (!result) {
        glGetProgramInfoLog(this -> programHandle , 512, NULL, message);
        std::cerr<<"error :  program link failed  :"<<message<<std::endl;
        return result;
    }
    
    //删除着色器程序
    glDeleteProgram(vertextShader);
    glDeleteProgram(fragmentShader);
    
    this -> compiled = true;
    return result;
}

void Shader::use() {
    if (this -> compiled == false) {
        return;
    }
    glUseProgram(this -> programHandle);
}


GLuint Shader::getProgramHandle() const {
    if (this -> compiled == false) {
        return 0;
    }
    return this -> programHandle;
}


void Shader::deleteProgramHandle() {
    if (this -> compiled == false) {
        return;
    }
    glDeleteProgram(this -> programHandle);
    this -> compiled = false;
}

GLint  Shader::getAttributeLocation(const std::string& name) {
    return glGetAttribLocation(this -> getProgramHandle(), name.c_str());
}


GLint Shader::getUniformLocation(const std::string& name) {
    return glGetUniformLocation(getProgramHandle(), name.c_str());
}


void Shader::setUniformVec3(const std::string& name, glm::vec3 value) {
    glUniform3fv(glGetUniformLocation(this -> getProgramHandle(), name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setUniformMatrix4(const std::string& name, glm::mat4 value) {
    glUniformMatrix4fv(glGetUniformLocation(this -> getProgramHandle(), name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniformInt(const std::string& name, int value) {
    GLint location = glGetUniformLocation(this -> getProgramHandle(), name.c_str());
    glUniform1i(location, value);
}


void Shader::setUniformFloat(const std::string& name, float value) {
    GLint location = glGetUniformLocation(this -> getProgramHandle(), name.c_str());
    glUniform1f(location, value);
}
