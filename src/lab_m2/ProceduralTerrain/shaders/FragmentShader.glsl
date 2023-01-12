#version 430

// Input
//layout(location = 0) in vec2 texture_coord;

// Output
layout(location = 0) out vec4 out_color;

uniform sampler2D grass;
uniform sampler2D cobble;
uniform sampler2D snow;

uniform float time;

in float height;
in vec2 textCoord;

in vec3 world_normal;
in vec3 world_position;

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

float get_light()
{
    float intensity = 0.15f; // ambiental

    vec4 L = vec4(0, 1, 0, 1);
    L = rotationMatrix(vec3(1, 0, 0),radians(30 * time)) * L;
    

    vec3 N = normalize(world_normal);
    intensity += 0.75 * max(dot(N, L.xyz), 0);

    return intensity;
}

void main()
{
    float color_intensity = get_light();



    if (height < 0) {
        out_color = vec4(texture(cobble, textCoord).xyz * color_intensity, 1);

    } else if (height < 4) {
        vec4 c1 = vec4(texture(cobble, textCoord).xyz * color_intensity, 1);
        vec4 c2 = vec4(texture(grass, textCoord).xyz * color_intensity, 1);

        out_color = mix(c1, c2, (height) / 4);
    } else if (height < 6)
        out_color = vec4(texture(grass, textCoord).xyz * color_intensity, 1);
    else if (height < 8) {
        vec4 c1 = vec4(texture(grass, textCoord).xyz * color_intensity, 1);
        vec4 c2 = vec4(texture(snow, textCoord).xyz * color_intensity, 1);

        out_color = mix(c1, c2, (height - 6) / 2);

    } else
        out_color = vec4(texture(snow, textCoord).xyz * color_intensity, 1);
}
