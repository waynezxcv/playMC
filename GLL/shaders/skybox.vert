
#version 330 core

layout (location = 0) in vec3 inPosition;
out vec3 texCoord;

uniform mat4 projection;
uniform mat4 view;



void main() {
    
    texCoord = inPosition.xyz;
    /*
     对于view matrix需要过滤掉位移，只保留旋转，从而保证skybox用于以摄像机所在的位置为中心。
     */
    mat4 matrix = view;
    matrix[3][0] = 0.0f;
    matrix[3][1] = 0.0f;
    matrix[3][2] = 0.0f;
    
    vec4 pos = projection * matrix  * vec4(inPosition, 1.0);
    
    gl_Position = pos.xyww;
}
