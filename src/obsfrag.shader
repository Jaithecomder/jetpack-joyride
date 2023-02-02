#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoord;

uniform vec4 ourColor;

void main()
{
   FragColor = ourColor;
   BrightColor = ourColor;
};