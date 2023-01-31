#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D bg1;
uniform sampler2D bg2;


void main()
{
   FragColor = mix(texture(bg1, TexCoord), texture(bg2, TexCoord), 0.4);
};