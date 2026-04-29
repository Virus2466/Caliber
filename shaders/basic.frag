#version 460 core

in vec3 v_normal;
in vec3 v_fragPos;

out vec4 FragColor;

// ---------Light Strucuts -----------
struct DirectionalLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

}; 

struct PointLight{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};


struct SpotLight {
    vec3  position;
    vec3  direction;
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float cutOff;       // inner cone angle (cosine)
    float outerCutOff;  // outer cone angle (cosine)
    float constant;
    float linear;
    float quadratic;
};


// ----------- UNIFORMS ------------
uniform vec3 u_viewPos;
uniform vec3 u_objectColor;
uniform DirectionalLight u_dirLight;
uniform PointLight u_pointLight;
uniform SpotLight u_spotLight;


vec3 calcDirectional(DirectionalLight light , vec3 normal , vec3 viewDir);
vec3 calcPoint(PointLight light , vec3 normal , vec3 viewDir , vec3 fragPos);
vec3 calcSpot(SpotLight light , vec3 normal , vec3 viewDir , vec3 fragPos);



void main(){

    vec3 normal = normalize(v_normal);
    vec3 viewDir = normalize(u_viewPos - v_fragPos);
    
    vec3 result = vec3(0.0);
    result += calcDirectional(u_dirLight , normal , viewDir);
    result += calcPoint(u_pointLight , normal , viewDir , v_fragPos);
    result += calcSpot(u_spotLight , normal , viewDir , v_fragPos);

    FragColor = vec4(result * u_objectColor , 1.0);

}


vec3 calcDirectional(DirectionalLight light , vec3 normal , vec3 viewDir){
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal,lightDir),0.0);

    vec3 reflectDir = reflect(-lightDir , normal);
    float spec = pow(max(dot(viewDir,reflectDir),0.0),32.0);

    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    return ambient + diffuse + specular;
}


vec3 calcPoint(PointLight light , vec3 normal , vec3 viewDir , vec3 fragPos){
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    // attenuation — light gets dimmer with distance
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant +
                                light.linear    * distance +
                                light.quadratic * distance * distance);

    vec3 ambient  = light.ambient  * attenuation;
    vec3 diffuse  = light.diffuse  * diff * attenuation;
    vec3 specular = light.specular * spec * attenuation;

    return ambient + diffuse + specular;
}


vec3 calcSpot(SpotLight light, vec3 normal, vec3 viewDir, vec3 fragPos) {
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant +
                                light.linear    * distance +
                                light.quadratic * distance * distance);

    // cone intensity — smooth edge between inner and outer cone
    float theta     = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient  = light.ambient  * attenuation;
    vec3 diffuse  = light.diffuse  * diff * attenuation * intensity;
    vec3 specular = light.specular * spec * attenuation * intensity;

    return ambient + diffuse + specular;
}