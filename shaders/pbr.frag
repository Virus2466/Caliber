#version 460 core

in vec3 v_fragPos;
in vec3 v_normal;
in vec3 texCoord;


out vec4 v_fragColor;

// Material
uniform vec3 u_albedo;
uniform float u_metallic;
uniform float u_roughness;
uniform float u_ao;

// Lights
uniform vec3 u_lightPositions[4];
uniform vec3 u_lightColors[4];
uniform int u_lightCount;

uniform vec3 u_viewPos;

// Constants
const float PI = 3.14159265359;


// TODO : PBR Functions..
