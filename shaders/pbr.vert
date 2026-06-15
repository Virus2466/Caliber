#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
// layout (location = 4) in vec3 aBitangent;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_fragPos;
out vec3 v_normal;
out vec2 v_texCoord; 
out mat3 v_TBN;

void main(){
    gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0);
    v_fragPos = vec3(u_model * vec4(aPos , 1.0));
    
    mat3 normalMatrix = mat3(transpose(inverse(u_model)));
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(cross(N, T));

    v_normal = N;
    v_TBN = mat3(T, B, N);
    v_texCoord = aTexCoord; 
}
