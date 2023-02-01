#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D bg3;


void main()
{
   FragColor = texture(bg3, TexCoords);
};