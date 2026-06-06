#version 460 core

in vec3 v_fragPos;
in vec3 v_normal;
in vec2 v_texCoord;
in mat3 v_TBN;


out vec4 FragColor;

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

// texture maps
uniform sampler2D u_albedoMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_metallicRoughnessMap;
uniform sampler2D u_aoMap;

// flags
uniform bool u_hasAlbedoMap;
uniform bool u_hasNormalMap;
uniform bool u_hasMetallicRoughnessMap;
uniform bool u_hasAOMap;


// Constants
const float PI = 3.14159265359;


// Normal Distribution 
float distributionGGX(vec3 N , vec3 H ,  float roughness){
    float a = roughness * roughness;
    float a2 = a*a;
    float NdotH = max(dot(N,H),0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;

}

// Geometry — microfacet self shadowing
float geometrySchilkGGX(float NdotV , float roughness){
    float r = roughness + 1.0;
    float k = (r*r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);

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
void main(){

    vec3 albedo = u_hasAlbedoMap ? pow(texture(u_albedoMap , v_texCoord).rgb , vec3(2.2)) : u_albedo;
    float metallic = u_hasMetallicRoughnessMap ? texture(u_metallicRoughnessMap , v_texCoord).b : u_metallic;
    float roughness = u_hasMetallicRoughnessMap ? texture(u_metallicRoughnessMap , v_texCoord).g : u_roughness;
    float ao = u_hasAOMap ? texture(u_aoMap , v_texCoord).r : u_ao;

    metallic = clamp(metallic, 0.0, 1.0);
    roughness = clamp(roughness, 0.18, 1.0);
    ao = clamp(ao, 0.0, 1.0);




    vec3 N = normalize(v_normal);
    if(u_hasNormalMap){
        vec3 tangentNormal = texture(u_normalMap , v_texCoord).rgb * 2.0 - 1.0;
        N = normalize(v_TBN * tangentNormal);
    }

    vec3 V = normalize(u_viewPos - v_fragPos);


    // F0 -> base reflectivity
    // non metals uses 0.04 metals uses their color
    vec3 F0 = vec3(0.04);
    F0 = mix(F0 , albedo , metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    for(int i = 0 ; i < u_lightCount; i++){
        vec3 L = normalize(u_lightPositions[i] - v_fragPos);
        vec3 H = normalize(V+L);

        // attenuation
        float distance = length(u_lightPositions[i] - v_fragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = u_lightColors[i] * attenuation;

        // Cook Torrence BRDF
        float D = distributionGGX(N,H,roughness);
        float G = geometrySmith(N,V,L,roughness);
        vec3 F = fresnelSchilk(max(dot(H,V),0.0),F0);

        // specular
        vec3 numerator = D * G * F;
        float denominator = 4.0 * max(dot(N,V),0.0) * max(dot(N,L),0.0) + 0.0001;
        vec3 specular = numerator / denominator;


        // kd -> diffuse contribution
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= (1.0 - metallic);

        float NdotL = max(dot(N,L),0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // ambient
    vec3 ambient = vec3(0.18) * albedo * ao;
    vec3 color = ambient + Lo;

    // HDR tonemapping - compress bright values without blowing broad areas to white.
    color = vec3(1.0) - exp(-color * 0.9);

    // gamma correction
    color = pow(color , vec3(1.0/2.2));

    FragColor = vec4(color , 1.0);



}
