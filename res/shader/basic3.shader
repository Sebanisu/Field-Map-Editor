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

layout(location = 0) out vec4 color;
layout(location = 1) out uvec4 uout_mask;
layout(location = 2) out vec4 out_mask;
layout(location = 3) out int tile_id;

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

uniform sampler2D u_Textures[32];

uniform vec4 u_Tint;
uniform vec2 u_Grid;

uvec4 uvec4_encode_uint_to_rgba(uint id) {
    return uvec4(
        (id >> 24) & 0xFFu,
        (id >> 16) & 0xFFu,
        (id >>  8) & 0xFFu,
        id & 0xFFu
    );
}

vec4 vec4_encode_uint_to_rgba(uint id) {
    // Extract bits according to your layout
    uint r = (id >> 21u) & 0x7FFu;   // 11 bits: bits 21..31
    uint g = (id >> 10u) & 0x7FFu;   // 11 bits: bits 10..20
    uint b = id & 0x3FFu;             // 10 bits: bits 0..9

    return vec4(
        float(r) / 2047.0,  // Normalize 11 bits to [0,1]
        float(g) / 2047.0,  // Normalize 11 bits to [0,1]
        float(b) / 1023.0,  // Normalize 10 bits to [0,1]
        1.0                 // Full alpha for visibility
    );
}

void main()
{
    tile_id = v_tile_id;

    // Sample texture color
    uint index = uint(v.texture);
    vec4 texColor = texture(u_Textures[index], v.TexCoord * v.tiling_factor);
    ivec2 textureSize2d = textureSize(u_Textures[index], 0);

    // Only write mask outputs if alpha > 0
    if (texColor.a > 0.0)
    {
        uout_mask  = uvec4_encode_uint_to_rgba(v_pupu_id);
        out_mask = vec4_encode_uint_to_rgba(v_pupu_id);
    }

    if (u_Grid.y > 0 && u_Grid.x > 0)
    {
        vec2 grid_size = vec2(float(textureSize2d.x)/u_Grid.x, float(textureSize2d.y)/u_Grid.y);
        float y = fract(v.TexCoord.y * grid_size.y);
        float x = fract(v.TexCoord.x * grid_size.x);
        float y2 = fract((1.0 - v.TexCoord.y) * grid_size.y);
        float x2 = fract((1.0 - v.TexCoord.x) * grid_size.x);

        if ((y > 0.95 || x > 0.95) || (y2 > 0.95 || x2 > 0.95))
        {
            color = u_Tint * v.color;
        }
        else
        {
            color = texColor * u_Tint * v.color;
        }
    }
    else
    {
        color = texColor * u_Tint * v.color;
    }
}