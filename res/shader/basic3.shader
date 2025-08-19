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
precision highp float; // Ensure high precision for calculations
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 out_mask;
layout(location = 2) out int tile_id;

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

//https://github.com/hughsk/glsl-hsv2rgb/blob/master/index.glsl
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec4 vec4_encode_uint_to_rgba(uint id) {
    float hue = fract(float(id) * 0.61803398875);
    vec3 temp_color = hsv2rgb(vec3(hue, 0.8, 0.9));
    // Round to nearest integer to match C++ conversion
    //vec3 rounded_color = floor(temp_color * 255.0 + 0.5) / 255.0;
    //return vec4(rounded_color, 1.0);
    return vec4(temp_color, 1.0);
}

void main()
{
    tile_id = v_tile_id;

    // Sample texture color
    uint index = uint(v.texture);
    vec4 texColor = texture(u_Textures[index], v.TexCoord * v.tiling_factor);
    ivec2 textureSize2d = textureSize(u_Textures[index], 0);

    // Only write mask outputs if alpha > 0
    if (texColor.a != 0.0)
    {
        out_mask  = vec4_encode_uint_to_rgba(v_pupu_id);
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