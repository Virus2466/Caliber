#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_fragPos;
out vec3 v_normal;
out vec2 v_texCoord; 
out vec4 v_tangent;

void main(){
    gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0);
    v_fragPos = vec3(u_model * vec4(aPos , 1.0));
    
    mat3 model = mat3(u_model);
    v_normal = normalize(transpose(inverse(model)) * aNormal);
    vec3 T = model * aTangent;
    float lengthT = length(T);
    // Assimp encodes glTF tangent.w in the imported bitangent. Recover its sign
    // in world space so mirrored UV islands and reflected instances both work.
    float handedness = sign(dot(cross(v_normal, T), model * aBitangent));
    v_tangent = vec4(lengthT > 0.0 ? T / lengthT : vec3(0.0), handedness);
    v_texCoord = aTexCoord; 
}
