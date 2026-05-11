#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;

void main()
{
    FragPos = vec3(u_Model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;
    
    vec3 T = normalize(mat3(u_Model) * aTangent);
    vec3 N = normalize(Normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);
    
    gl_Position = u_Projection * u_View * vec4(FragPos, 1.0);
}
