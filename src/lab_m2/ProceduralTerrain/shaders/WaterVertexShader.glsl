#version 430

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec4 CurvatureExp;
uniform vec3 CameraWorldPos;

// Output
layout(location = 0) out vec2 texture_coord;


void main()
{
    texture_coord = v_texture_coord;

    vec4 vertex_pos = Model * vec4(v_position, 1.0);
    
    float dist = distance(CameraWorldPos, vertex_pos.xyz);
    vertex_pos -= (dist * dist) * CurvatureExp;

    gl_Position = Projection * View * vec4(vertex_pos.xyz, 1.0);
}
