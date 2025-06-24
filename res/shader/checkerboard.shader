#shader vertex
#version 450 core

layout (location = 0) in vec2 in_pos;

layout (location = 0) out vec2 frag_uv;

void main()
{
    frag_uv = in_pos;// * 0.5 + 0.5; // Map from [-1,1] to [0,1]
    gl_Position = vec4(in_pos, 0.0, 1.0);
}

#shader fragment
#version 450 core

layout (location = 0) in vec2 frag_uv;

layout (location = 0) out vec4 frag_color;

layout (location = 0) uniform float tile_size;
layout (location = 1) uniform vec4 color1;
layout (location = 2) uniform vec4 color2;

void main()
{
    float x = floor(frag_uv.x / tile_size);
    float y = floor(frag_uv.y / tile_size);
    bool is_even = mod(x + y, 2.0) < 1.0;
    frag_color = is_even ? color1 : color2;
}
