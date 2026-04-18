#version 460 core

// input vertex data
layout (location = 0) in vec3 aPos;

void main(){
    gl_Position = vec4(aPos , 1.0);
}