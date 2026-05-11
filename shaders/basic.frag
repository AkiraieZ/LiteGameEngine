#version 450 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform vec3 u_Albedo;
uniform vec3 u_ViewPos;
uniform vec3 u_LightPositions[4];
uniform vec3 u_LightColors[4];
uniform float u_LightIntensities[4];
uniform int u_LightCount;
uniform mat4 u_LightSpaceMatrix;
uniform sampler2D u_ShadowMap;
uniform int u_UseShadows;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    float shadow = 0.0;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);
    float currentDepth = projCoords.z;
    
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    if (projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(u_ViewPos - FragPos);
    
    vec3 result = vec3(0.1) * u_Albedo;
    
    for (int i = 0; i < u_LightCount; i++) {
        vec3 lightDir = normalize(u_LightPositions[i] - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        
        float distance = length(u_LightPositions[i] - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        
        float diffuse = max(dot(norm, lightDir), 0.0);
        float specular = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
        
        float shadow = 0.0;
        if (u_UseShadows == 1 && i == 0) {
            shadow = ShadowCalculation(FragPosLightSpace, norm, lightDir);
        }
        
        vec3 lightContribution = (diffuse * (1.0 - shadow) + specular * (1.0 - shadow)) * u_LightColors[i] * u_LightIntensities[i] * attenuation;
        result += lightContribution * u_Albedo;
    }
    
    FragColor = vec4(result, 1.0);
}
