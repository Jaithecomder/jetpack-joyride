#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform int glow;
uniform sampler2D wiz;

void main()
{
   FragColor = texture(wiz, TexCoords);
   if(glow != 0)
   {
      if(FragColor.r > 0.6 && FragColor.g > 0.6 && FragColor.b < 0.2)
      {
         BrightColor = vec4(FragColor.rgb, 1.0);
      }
      else
      {
         BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
      }
   }
   else
   {
      BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
   }
};