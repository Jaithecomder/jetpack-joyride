#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D bg1;
uniform sampler2D bg2;


void main()
{
   FragColor = mix(texture(bg1, TexCoords), texture(bg2, TexCoords), 0.4);
};