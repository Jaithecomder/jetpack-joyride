#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform vec4 ourColor;

void main()
{
   FragColor = ourColor;
};