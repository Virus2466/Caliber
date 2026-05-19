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


// Normal Distribution 
float distributionGGX(vec3 N , vec3 H ,  float roughness){
    float a = roughness * roughness;
    float a2 = a*a;
    float NdotH = max(dot(N,H),0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;

}

// Geometry — microfacet self shadowing
float geometrySchilkGGX(float NdotV , float roughness){
    float r = roughness + 1.0;
    float k = (r*r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) * k);

}

float geometrySmith(vec3 N , vec3 V , vec3 L ,float roughness){
    float NdotV = max(dot(N,V) , 0.0);
    float NdotL = max(dot(N,L) , 0.0);
    float ggx1 = geometrySchilkGGX(NdotV , roughness);
    float ggx2 = geometrySchilkGGX(NdotL , roughness);
    return ggx1 * ggx2;
}

// Fresnel — reflectance at grazing angles
vec3 fresnelSchilk(float cosTheta , vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta , 0.0 , 1.0),5.0);
}


// Main
void main(){}