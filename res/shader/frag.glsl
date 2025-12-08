#version 460 core

out vec4 FragColor;
in vec3 sharedColor;
in vec2 TextCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TextCoord);
    // FragColor = texture(ourTexture, TextCoord) * vec4(sharedColor, 1.0);
}