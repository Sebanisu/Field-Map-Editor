#shader vertex
#version 450

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in float a_texture;
layout(location = 4) in float a_tiling_factor;
layout(location = 5) in int a_tile_id;

struct vertex_output
{
  float texture;
  vec2 TexCoord;
  vec4 color;
};
layout(location = 0) out vertex_output v;
layout(location = 3) out flat int v_tile_id;
uniform mat4 u_MVP;

void main()
{
  gl_Position = u_MVP * vec4(a_position, 1.0);
  v.TexCoord = a_texCoord * a_tiling_factor;
  v.texture = a_texture;
  v.color = a_color;
  v_tile_id = a_tile_id;
}
  #shader fragment
  #version 450

//layout(location = 0) out vec4 color;
layout(location = 1) out int tile_id;

struct vertex_output
{
  float texture;
  vec2 TexCoord;
  vec4 color;
};
layout(location = 0) in vertex_output v;
layout(location = 3) in flat int v_tile_id;

uniform highp isampler2D u_Textures[32];

void main()
{
  int index = int(v.texture);
  ivec4 texColor = texture(u_Textures[index], v.TexCoord);
  tile_id = texColor.r;
}