#shader vertex
#version 330 core

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in float a_texture;
layout(location = 4) in float a_tiling_factor;

out float v_texture;
out vec2 v_TexCoord;
out vec4 v_color;
out float v_tiling_factor;
uniform mat4 u_MVP;

void main()
{
  gl_Position = u_MVP * a_position;
  v_TexCoord = a_texCoord;
  v_texture = a_texture;
  v_tiling_factor = a_tiling_factor;
  v_color = a_color;
}
  #shader fragment
  #version 330 core

layout(location = 0) out vec4 color;

in float v_texture;
in vec2 v_TexCoord;
in vec4 v_color;
in float v_tiling_factor;

uniform sampler2D u_Textures[32];

uniform vec4 u_Color;
uniform vec2 u_Grid;
//uniform sampler2D u_Texture;

void main()
{
  int index = int(v_texture);
  vec4 texColor = texture(u_Textures[index], v_TexCoord * v_tiling_factor);
  ivec2 textureSize2d = textureSize(u_Textures[index], 0);

  if (u_Grid.y > 0 && u_Grid.x > 0)
  {
    vec2 grid_size = vec2(float(textureSize2d.x)/u_Grid.x, float(textureSize2d.y)/u_Grid.y);
    float y = fract(v_TexCoord.y * grid_size.y);
    float x = fract(v_TexCoord.x * grid_size.x);
    float y2 = fract((1.0 - v_TexCoord.y) * grid_size.y);
    float x2 = fract((1.0 - v_TexCoord.x) * grid_size.x);

    if ((y > 0.95 || x > 0.95) || (y2 > 0.95 || x2 > 0.95))
    {
      color = u_Color * v_color;
    }
    else
    {
      color =texColor * u_Color * v_color;
    }
  }
  else
  {
    color = texColor * u_Color * v_color;
  }
}
