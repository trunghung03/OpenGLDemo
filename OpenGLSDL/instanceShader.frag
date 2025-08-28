#version 330 core
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    vec3 color_diffuse;
    vec3 color_specular;
    float     shininess;
};

uniform Material material;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
    //FragColor = texture(texture_diffuse1, TexCoords);
    FragColor = vec4(material.color_diffuse, 1.0);
}