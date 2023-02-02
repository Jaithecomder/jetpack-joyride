#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform vec4 ourColor;

void main()
{
   FragColor = ourColor;
   BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
};