#version 330 core


//材质
struct Material {
    
    sampler2D diffuseSampler;
    sampler2D specularSampler;
    sampler2D normalSampler;
    sampler2D ambientSampler;
    
    float shininess;
};

// 光源
struct LightCaster {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};



in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec3 fragTangent;
in vec3 fragBitangent;


uniform vec3 cameraPosition;
uniform Material material;
uniform LightCaster light;

out vec4 fragColor;


void main () {
    
    // 环境光
    vec3 ambient = light.ambient * texture(material.diffuseSampler, fragTexCoords).rgb;
    
    // 漫反射
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(light.position - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuseSampler, fragTexCoords).rgb;
    
    // 镜面反射
    vec3 viewDir = normalize(cameraPosition - fragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specularSampler, fragTexCoords).rgb;
    
    // 聚光灯
    float theta = dot(lightDir, normalize(- light.direction));
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;
    
    // 光照按距离衰减
    float distance = length(light.position - fragPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    
    vec3 result = ambient + diffuse + specular;
    
    fragColor = vec4(result, 1.0);
}


