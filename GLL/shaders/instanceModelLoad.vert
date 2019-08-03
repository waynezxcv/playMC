#version 330 core


uniform mat4 projection;
uniform mat4 view;


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;


// 顶点属性最多vec4 输入 实际上有4个vec4输入构造这个mat4
layout (location = 5) in mat4 aInstanceMatrix;



out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoords;
out vec3 fragTangent;
out vec3 fragBitangent;



void main() {
    // model 矩阵在instance draw下，用顶点着色器传进来
    mat4 model = aInstanceMatrix;
    
    fragPosition =  vec3(model * vec4(aPos, 1.0));
    fragNormal = mat3(transpose(inverse(model))) * aNormal;
    fragTexCoords = aTexCoords;
    fragTangent = aTangent;
    fragBitangent = aBitangent;
    
    // 顶点位置
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
