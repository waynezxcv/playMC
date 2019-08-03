
#version 330 core

layout (location = 0) in vec3 inVertextPostion;
layout (location = 1) in vec2 inTexCoords;
layout (location = 2) in float inCardinalLight;
layout (location = 3) in vec3 inNormal;
layout (location = 4) in vec3 inOffset;

out vec2 passTexCoords;
out float passCardinalLight;

uniform mat4 projection;
uniform mat4 view;

void main() {
    passTexCoords = inTexCoords;
    passCardinalLight = inCardinalLight;
    gl_Position =  projection * view * vec4 (inVertextPostion.x  + inOffset.x , inVertextPostion.y + inOffset.y, inVertextPostion.z + inOffset.z, 1.0);
}


