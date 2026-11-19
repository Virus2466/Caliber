#version 460 core

in vec3 v_fragPos;
in vec3 v_normal;
in vec2 v_texCoord;
in vec4 v_tangent;


out vec4 FragColor;

// Material
uniform vec3 u_albedo;
uniform float u_metallic;
uniform float u_roughness;
uniform float u_ao;
uniform vec3 u_baseColorFactor;
uniform float u_metallicFactor;
uniform float u_roughnessFactor;
uniform float u_occlusionStrength;

// Lights
uniform vec3 u_lightPositions[4];
uniform vec3 u_lightColors[4];
uniform int u_lightCount;
// Direction toward the sun; sunlight has no distance attenuation.
uniform vec3 u_sunDirection;
uniform vec3 u_sunRadiance;
uniform float u_exposure;

uniform vec3 u_viewPos;

// texture maps
uniform sampler2D u_albedoMap;
uniform sampler2D u_normalMap;
uniform float u_normalScale = 1.0;
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

// Use the same Cook-Torrance response for point lights and sunlight.
vec3 evaluateLight(vec3 N, vec3 V, vec3 L, vec3 radiance,
                   vec3 albedo, float metallic, float roughness, vec3 F0) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    if (NdotV <= 0.0 || NdotL <= 0.0) return vec3(0.0);

    vec3 H = normalize(V + L);
    float D = distributionGGX(N, H, roughness);
    float G = geometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchilk(max(dot(H, V), 0.0), F0);
    vec3 specular = (D * G * F) / (4.0 * NdotV * NdotL + 0.0001);
    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}


// Main
void main(){

    // The sRGB albedo texture decodes during sampling. The other maps are linear data.
    vec3 albedo = u_albedo * u_baseColorFactor;
    if (u_hasAlbedoMap) albedo *= texture(u_albedoMap, v_texCoord).rgb;
    vec2 roughMetal = u_hasMetallicRoughnessMap ? texture(u_metallicRoughnessMap, v_texCoord).gb : vec2(1.0);
    float metallic = u_metallic * u_metallicFactor * roughMetal.y;
    float roughness = u_roughness * u_roughnessFactor * roughMetal.x;
    float ao = u_ao;
    if (u_hasAOMap) ao *= mix(1.0, texture(u_aoMap, v_texCoord).r, u_occlusionStrength);

    metallic = clamp(metallic, 0.0, 1.0);
    roughness = clamp(roughness, 0.18, 1.0);
    ao = clamp(ao, 0.0, 1.0);




    vec3 N = normalize(v_normal);
    if (u_hasNormalMap) {
        vec3 tangentNormal = texture(u_normalMap, v_texCoord).rgb * 2.0 - 1.0;
        tangentNormal.xy *= u_normalScale;
        vec3 T = v_tangent.xyz - N * dot(N, v_tangent.xyz);
        float lengthSquared = dot(T, T);
        if (lengthSquared > 1e-10 && abs(v_tangent.w) > 0.5) {
            T *= inversesqrt(lengthSquared);
            vec3 B = sign(v_tangent.w) * cross(N, T);
            vec3 mappedNormal = mat3(T, B, N) * tangentNormal;
            if (dot(mappedNormal, mappedNormal) > 1e-10) N = normalize(mappedNormal);
        }
    }
    if (!gl_FrontFacing) N = -N;


    vec3 V = normalize(u_viewPos - v_fragPos);


    // F0 -> base reflectivity
    // non metals uses 0.04 metals uses their color
    vec3 F0 = vec3(0.04);
    F0 = mix(F0 , albedo , metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    for(int i = 0 ; i < clamp(u_lightCount, 0, 4); i++){
        vec3 offset = u_lightPositions[i] - v_fragPos;
        float distanceSquared = max(dot(offset, offset), 0.0001);
        vec3 L = offset * inversesqrt(distanceSquared);
        vec3 radiance = u_lightColors[i] / distanceSquared;
        Lo += evaluateLight(N, V, L, radiance, albedo, metallic, roughness, F0);
    }

    Lo += evaluateLight(N, V, u_sunDirection, u_sunRadiance,
                        albedo, metallic, roughness, F0);

    // ambient
    vec3 ambient = vec3(0.18) * albedo * ao;
    vec3 color = ambient + Lo;

    // HDR tonemapping - compress bright values without blowing broad areas to white.
    color = vec3(1.0) - exp(-color * u_exposure);

    // gamma correction
    color = pow(color , vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);


}
