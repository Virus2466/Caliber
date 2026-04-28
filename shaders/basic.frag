#version 460 core

in vec3 v_normal;
in vec3 v_fragPos;


// output color
out vec4 FragColor;

uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform vec3 u_objectColor;
uniform vec3 u_viewPos;


void main(){
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * u_lightColor;

    // Diffuse 
    vec3 norm  = normalize(v_normal);
    vec3 lightDir = normalize(u_lightPos - v_fragPos);
    float diff = max(dot(norm , lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_viewPos - v_fragPos);
    vec3 reflectDir = reflect(-lightDir , norm);
    float spec = pow(max(dot(viewDir , reflectDir ), 0.0),32.0);
    vec3 specular = specularStrength * spec * u_lightColor;

    vec3 result = (ambient + diffuse + specular) * u_objectColor;

    FragColor = vec4(result , 1.0); 
}