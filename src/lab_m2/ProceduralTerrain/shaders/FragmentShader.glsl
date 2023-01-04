#version 430

// Input
//layout(location = 0) in vec2 texture_coord;

// Output
layout(location = 0) out vec4 out_color;

uniform sampler2D grass;
uniform sampler2D cobble;

in float height;
in vec2 textCoord;

void main()
{
    //out_color = vec4(height / 10, height / 10, height / 10, 1);
    if (height < 0)
        out_color = texture(cobble, textCoord);
    else
        out_color = texture(grass, textCoord);
}
