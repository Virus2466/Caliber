#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;


out vec3 v_normal;
out vec3 v_fragPos;


void main() {
    gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0);
    v_fragPos = vec3(u_model * vec4(aPos , 1.0));
    v_normal = mat3(transpose(inverse(u_model))) * aNormal;
}