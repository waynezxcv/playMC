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
    inVert.x += sin((globalTime + inVert.z + inVert.y) * 1.8f) / 15.0f;
    inVert.z -= cos((globalTime + inVert.x + inVert.y) * 1.8f) / 15.0f;
    return vec4(inVert, 1);
}


void main() {
    passTexCoords = inTexCoords;
    passCardinalLight = inCardinalLight;
    gl_Position = projection * view * getWorldPos();
}



