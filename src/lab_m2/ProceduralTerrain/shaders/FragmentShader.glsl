#version 430

// Input
//layout(location = 0) in vec2 texture_coord;

// Output
layout(location = 0) out vec4 out_color;

in float height;

void main()
{
    out_color = vec4(height / 10, height / 10, height / 10, 1);
}
