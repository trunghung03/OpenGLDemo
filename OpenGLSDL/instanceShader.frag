#version 330 core
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    vec3 color_diffuse;
    vec3 color_specular;
    float     shininess;
};
uniform Material material;

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;

vec3 CalcDirLightSolid(DirLight light, vec3 normal, vec3 viewDir);

void main()
{
    // properties 
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // phase 1: Directional lighting
    vec3 result = CalcDirLightSolid(dirLight, norm, viewDir);
    
    FragColor = vec4(result, 1.0);
}


vec3 CalcDirLightSolid(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * material.color_diffuse;
    vec3 diffuse  = light.diffuse  * diff * material.color_diffuse;
    vec3 specular = light.specular * spec * material.color_specular;
    return (ambient + diffuse + specular);
} 