#version 430

// Input and output topologies
layout(points) in;
layout(triangle_strip, max_vertices = 256) out;

// Input
uniform sampler2D texture_1;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec2 textPos;
uniform float textSize;

uniform int minHeight;
uniform int maxHeight;

out float height;

/*
2 (-, +)    3(+, +)

0 (-, -)    1(+, -)
*/

void EmitSquare(vec3 pos, float size, float h0, float h1, float h2, float h3)
{
    gl_Position = Projection * View * Model * vec4(pos + vec3(-size, h0, -size), 1.0);
    height = h0; EmitVertex();

    gl_Position = Projection * View * Model * vec4(pos + vec3(size, h1, -size), 1.0);
    height = h1; EmitVertex();

    gl_Position = Projection * View * Model * vec4(pos + vec3(-size, h2, size), 1.0);
    height = h2; EmitVertex();
    EndPrimitive();

    gl_Position = Projection * View * Model * vec4(pos + vec3(-size, h2, size), 1.0);
    height = h2; EmitVertex();

    gl_Position = Projection * View * Model * vec4(pos + vec3(size, h1, -size), 1.0);
    height = h1; EmitVertex();

    gl_Position = Projection * View * Model * vec4(pos + vec3(size, h3, size), 1.0);
    height = h3; EmitVertex();
    EndPrimitive();
}

void main()
{
    int rez = 4;
    float myStep = 1.0 / rez;
    float myStepText = textSize / rez;

    int diffHeight = maxHeight - minHeight;

    for (float x = -0.5, tx = textPos.x; x <= 0.5; x += myStep, tx += myStepText)
    {
        for (float y = -0.5, ty = textPos.y; y <= 0.5; y += myStep, ty += myStepText)
        {
            float h0 = diffHeight * texture(texture_1, vec2(tx - myStepText / 2, ty - myStepText / 2)).x + minHeight;
            float h1 = diffHeight * texture(texture_1, vec2(tx + myStepText / 2, ty - myStepText / 2)).x + minHeight;
            float h2 = diffHeight * texture(texture_1, vec2(tx - myStepText / 2, ty + myStepText / 2)).x + minHeight;
            float h3 = diffHeight * texture(texture_1, vec2(tx + myStepText / 2, ty + myStepText / 2)).x + minHeight;

            EmitSquare(vec3(x, 0, y), myStep / 2, h0, h1, h2, h3);
        }
    }
}
