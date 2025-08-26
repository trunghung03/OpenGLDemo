#version 330 core

in vec4 FragPos;

out vec4 FragColor;

void main()
{
    float noise = (FragPos.y) / 7.5;
    FragColor = vec4(0.0, noise, 1.0 - noise, 1.0);
}