
#version 330 core


out vec4 outColor;
in vec2 passTexCoords;
in float passCardinalLight;
uniform sampler2D texSampler;
vec4 color;


void main() {
    color = texture(texSampler, passTexCoords);
    outColor = color * passCardinalLight;
    if (outColor.a == 0) discard;
}

