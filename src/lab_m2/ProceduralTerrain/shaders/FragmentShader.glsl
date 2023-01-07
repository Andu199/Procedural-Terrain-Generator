#version 430

// Input
//layout(location = 0) in vec2 texture_coord;

// Output
layout(location = 0) out vec4 out_color;

uniform sampler2D grass;
uniform sampler2D cobble;
uniform sampler2D snow;

uniform vec3 sun_position;

in float height;
in vec2 textCoord;

in vec3 world_normal;
in vec3 world_position;

float get_light()
{
    float intensity = 0.15f; // ambiental

    vec3 L = normalize(sun_position - world_position);
    vec3 N = normalize(world_normal);
    intensity += 0.75 * max(dot(N, L), 0);

    return intensity;
}

void main()
{
    float color_intensity = get_light();

    if (height < -2)
        out_color = vec4(texture(cobble, textCoord).xyz * color_intensity, 1);
    else if (height < 3)
        out_color = vec4(texture(grass, textCoord).xyz * color_intensity, 1);
    else
        out_color = vec4(texture(snow, textCoord).xyz * color_intensity, 1);
}
