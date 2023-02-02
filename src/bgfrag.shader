#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform sampler2D bg1;
uniform sampler2D bg2;


void main()
{
   FragColor = mix(texture(bg1, TexCoords), texture(bg2, TexCoords), 0.4);
   BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
};