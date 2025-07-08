#shader vertex
#version 450 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_texCoord;
layout (location = 3) in float a_texture;
layout (location = 4) in float a_tiling_factor;
layout (location = 5) in int a_tile_id;

layout (location = 0) out vec2 frag_uv;

layout (location = 3) uniform mat4 u_MVP;

void main()
{
    frag_uv = a_position.xy; // Treating position as UV source
    gl_Position = u_MVP * vec4(a_position, 1.0);
}

#shader fragment
#version 450 core

layout (location = 0) in vec2 frag_uv;

layout (location = 0) out vec4 frag_color;

layout (location = 0) uniform float tile_size;
layout (location = 1) uniform vec4 color1;
layout (location = 2) uniform vec4 color2;

// Even if unused, required for batch renderer compatibility
layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
    float x = floor(frag_uv.x / tile_size);
    float y = floor(frag_uv.y / tile_size);
    bool is_even = mod(x + y, 2.0) < 1.0;
    frag_color = is_even ? color1 : color2;
}
