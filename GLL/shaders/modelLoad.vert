#version 330 core


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoords;
out vec3 fragTangent;
out vec3 fragBitangent;


void main (void) {
    
    fragPosition =  vec3(model * vec4(aPos, 1.0));
    fragNormal = mat3(transpose(inverse(model))) * aNormal;
    fragTexCoords = aTexCoords;
    fragTangent = aTangent;
    fragBitangent = aBitangent;
    
    // 顶点位置
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
