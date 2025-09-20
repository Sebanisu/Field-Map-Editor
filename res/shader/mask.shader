#shader vertex
#version 450

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in float a_texture;
layout(location = 4) in float a_tiling_factor;
layout(location = 5) in int a_tile_id;
layout(location = 6) in uint a_pupu_id;

struct vertex_output
{
  float texture;
  vec2 TexCoord;
  vec4 color;
  float tiling_factor;
};
layout(location = 0) out vertex_output v;
layout(location = 4) out flat int v_tile_id;
layout(location = 5) out flat uint v_pupu_id;
uniform mat4 u_MVP;

void main()
{
  gl_Position = u_MVP * vec4(a_position,1.0);
  v.TexCoord = a_texCoord;
  v.texture = a_texture;
  v.tiling_factor = a_tiling_factor;
  v.color = a_color;
  v_tile_id = a_tile_id;
  v_pupu_id = a_pupu_id;
}
  #shader fragment
  #version 450
layout(location = 0) out vec4 Fragcolor;

struct vertex_output
{
  float texture;
  vec2 TexCoord;
  vec4 color;
  float tiling_factor;
};
layout(location = 0) in vertex_output v;
layout(location = 4) in flat int v_tile_id;
layout(location = 5) in flat uint v_pupu_id;

uniform sampler2D mainTex;
uniform sampler2D maskTex;

void main()
{
    vec4 maskColor = texture(mainTex, v.TexCoord);
    vec4 texColor = texture(maskTex, v.TexCoord);
    if (maskColor.r < 0.001 && maskColor.g < 0.001 && maskColor.b < 0.001)
        discard;
    Fragcolor = texColor;    
}