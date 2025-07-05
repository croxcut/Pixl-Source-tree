#version 460 core

out vec4 FragColor;
in vec3 sharedColor;

void main()
{
    FragColor = vec4(sharedColor, 1.0);
}