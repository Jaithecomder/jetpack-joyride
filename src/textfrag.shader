#version 330 core

in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D text;
uniform vec4 ourColor;

void main()
{
   FragColor = vec4(ourColor.xyz, texture(text, TexCoords).r);
};