#version 330

layout (location = 0) in vec3 inVertextPostion;
layout (location = 1) in vec2 inTexCoords;
layout (location = 2) in float inCardinalLight;
layout (location = 3) in vec3 inNormal;
layout (location = 4) in vec3 inOffset;

out vec2 passTexCoords;
out float passCardinalLight;

uniform mat4 projection;
uniform mat4 view;
uniform float globalTime;


vec4 getWorldPos() {
    vec3 inVert = vec3(inVertextPostion.x + inOffset.x , inVertextPostion.y + inOffset.y, inVertextPostion.z + inOffset.z);
    inVert.y += sin((globalTime + inVert.x) * 1.5) / 8.8f;
    inVert.y += cos((globalTime + inVert.z) * 1.5) / 8.1f;
    inVert.y -= 0.2;
    return vec4(inVert, 1);
}


void main() {
    passTexCoords = inTexCoords;
    passCardinalLight = inCardinalLight;
    gl_Position =  projection * view  * getWorldPos();
    
}
