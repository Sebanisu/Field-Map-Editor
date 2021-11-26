#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in float texture;

out float v_texture;
out vec2 v_TexCoord;
out vec4 v_color;
uniform mat4 u_MVP;

void main()
{
  gl_Position = u_MVP * position;
  v_TexCoord = texCoord;
  v_texture = texture;
  v_color = color;
}
  #shader fragment
  #version 330 core

layout(location = 0) out vec4 color;

in float v_texture;
in vec2 v_TexCoord;
in vec4 v_color;

uniform sampler2D u_Textures[10];

uniform vec4 u_Color;
//uniform sampler2D u_Texture;

void main()
{
  int index = int(v_texture);
  vec4 texColor = texture(u_Textures[index], v_TexCoord);
  color = texColor * u_Color; //vec4(texColor.r,texColor.g,texColor.b,1.0); //u_Color * v_color *
}
