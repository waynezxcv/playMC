
#ifndef MatrixMaker_hpp
#define MatrixMaker_hpp

#include <stdio.h>

namespace GLL {
    class MatrixMaker {
        
    public:
        
        // 创建投影矩阵
        static glm::mat4 makeProjectionMatrix(const float& aspect,
        const float& fovAngle,
        const float& nearClippingDistance,
        const float& farClippngDistance) {
            return glm::perspective(glm::radians(fovAngle), aspect, nearClippingDistance, farClippngDistance);
        }
        
        // 创建观察矩阵
        static glm::mat4 makeViewMatrix(const glm::vec3& eyePosition, //眼睛位置
        const glm::vec3& eyeTarget, // 观察的目标方向
        const glm::vec3& eyeUpDirect) { //向上的方向
            return glm::lookAt(eyePosition, eyeTarget, eyeUpDirect);
        }
        
        // 创建观察矩阵
        static glm::mat4 makeViewMatrix(const glm::vec3& rotation, const glm::vec3& position) {
            glm::mat4 matrix(1.0f);
            matrix = glm::rotate(matrix, glm::radians(rotation.x), {1, 0, 0});
            matrix = glm::rotate(matrix, glm::radians(rotation.y), {0, 1, 0});
            matrix = glm::rotate(matrix, glm::radians(rotation.z), {0, 0, 1});
            matrix = glm::translate(matrix, -position);
            return matrix;
        }


        // 创建模型矩阵
        static glm::mat4 makeModelMatrix(const glm::vec3& rotation, const glm::vec3& position) {
            glm::mat4 matrix;
            matrix = glm::rotate(matrix, glm::radians(rotation.x), {1, 0, 0});
            matrix = glm::rotate(matrix, glm::radians(rotation.y), {0, 1, 0});
            matrix = glm::rotate(matrix, glm::radians(rotation.z), {0, 0, 1});
            matrix = glm::translate(matrix, position);
            return matrix;
        }
    };
}

#endif /* MatrixMaker_hpp */
