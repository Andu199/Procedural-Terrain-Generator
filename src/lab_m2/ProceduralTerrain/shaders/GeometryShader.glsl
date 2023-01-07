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
out vec2 textCoord;

out vec3 world_normal;
out vec3 world_position;

/*
2 (-, +)    3(+, +)

0 (-, -)    1(+, -)
*/

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec4 compute_normal(float h2, float h0, float h3, float size)
{
    vec4 normal = vec4(0, 1, 0, 1);
    mat4 rot = rotationMatrix(vec3(0, 0, 1), atan((h2 - h0) / 2 * size));
    normal = rot * normal;

    rot = rotationMatrix(vec3(1, 0, 0), atan((h2 - h3) / 2 * size));
    normal = rot * normal;

    return normalize(normal);
}

vec4 squareNormal(float h0, float h1, float h2, float h3, float size)
{
    vec4 myNormal = vec4(0, 1, 0, 1);

    float h01 = (h0 + h1) / 2;
    float h23 = (h2 + h3) / 2;

    float h02 = (h0 + h2) / 2;
    float h13 = (h1 + h3) / 2;

    float angleX = atan((h23 - h01) / (2 * size));
    float angleZ = atan((h02 - h13) / (2 * size));

    myNormal = rotationMatrix(vec3(1, 0, 0), angleX) * myNormal;
    myNormal = rotationMatrix(vec3(0, 0, 1), angleZ) * myNormal;

    return myNormal;
}

void EmitSquare(vec3 pos, float size, float h0, float h1, float h2, float h3)
{
    vec3 norm = squareNormal(h0, h1, h2, h3, size).xyz;

    gl_Position = Projection * View * Model * vec4(pos + vec3(-size, h0, -size), 1.0);
    world_position = vec3(Model * vec4(pos + vec3(-size, h0, -size), 1.0));
    world_normal = norm;
    height = h0; textCoord = vec2(0, 0); EmitVertex();

    gl_Position = Projection * View * Model * vec4(pos + vec3(size, h1, -size), 1.0);
    world_position = vec3(Model * vec4(pos + vec3(size, h1, -size), 1.0));
    world_normal = norm;
    height = h1; textCoord = vec2(1, 0); EmitVertex();

    gl_Position = Projection * View * Model * vec4(pos + vec3(-size, h2, size), 1.0);
    world_position = vec3(Model * vec4(pos + vec3(-size, h2, size), 1.0));
    world_normal = norm;
    height = h2; textCoord = vec2(0, 1); EmitVertex();
    EndPrimitive();

    gl_Position = Projection * View * Model * vec4(pos + vec3(-size, h2, size), 1.0);
    world_position = vec3(Model * vec4(pos + vec3(-size, h2, size), 1.0));
    world_normal = norm;
    height = h2; textCoord = vec2(0, 1); EmitVertex();

    gl_Position = Projection * View * Model * vec4(pos + vec3(size, h1, -size), 1.0);
    world_position = vec3(Model * vec4(pos + vec3(size, h1, -size), 1.0));
    world_normal = norm;
    height = h1; textCoord = vec2(1, 0); EmitVertex();

    gl_Position = Projection * View * Model * vec4(pos + vec3(size, h3, size), 1.0);
    world_position = vec3(Model * vec4(pos + vec3(size, h3, size), 1.0));
    world_normal = norm;
    height = h3; textCoord = vec2(1, 1); EmitVertex();
    EndPrimitive();
}

void main()
{
    int rez = 2;
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
