#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
//layout(location = 1) in vec2 texCoord;

//out vec2 v_TexCoord;
out vec4 v_color;
uniform mat4 u_MVP;

void main()
{
  gl_Position = u_MVP * position;
  //v_TexCoord = texCoord;
  v_color = color;
}
  #shader fragment
  #version 330 core

layout(location = 0) out vec4 color;
//in vec2 v_TexCoord;
in vec4 v_color;

uniform vec4 u_Color;
//uniform sampler2D u_Texture;

void main()
{
  //vec4 texColor = texture(u_Texture, v_TexCoord);
  color = u_Color * v_color;//u_Color*texColor;
}
