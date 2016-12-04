#version 300 es

smooth in highp vec3 fragment_position;
smooth in highp vec3 fragment_color;
smooth in highp vec2 fragment_texture;
smooth in highp vec3 fragment_normal;

layout(std140) uniform block_camera
{
    highp mat4 view_matrix;
    highp mat4 projection_matrix;
    highp vec3 view_position;
};

layout(std140) uniform block_mesh
{
    highp mat4 model_matrix;
};

layout(std140) uniform block_precomputed
{
    highp mat4 model_view_matrix;
    highp mat4 model_view_projection_matrix;
};

out highp vec3 final_color;


struct directional_light
{
    highp vec3 direction;
    highp vec3 color;
    highp float ambient_intensity;
    highp float diffuse_intensity;
    highp float specular_intensity;
};

void main()
{
    directional_light light;
    light.direction = normalize(vec3(-1.0f, -2.0f, -5.0f));
    light.color = vec3(1.0f, 1.0f, 1.0f);
    light.ambient_intensity = 0.2f;
    light.diffuse_intensity = 0.8f;
    light.specular_intensity = 0.2f;

    lowp float material_shininess = 4.f;


    /* Ambient light */
    highp vec3 ambient_color = light.color * light.ambient_intensity;

    /* Diffuse light */
    highp vec3 diffuse_color = light.color * light.diffuse_intensity;
    diffuse_color *= clamp(dot(normalize(fragment_normal), -light.direction), 0.0, 1.0);

    /* Specular light */
    highp vec3 view_direction = normalize(fragment_position - view_position);
    highp vec3 reflect_direction = reflect(-light.direction, normalize(fragment_normal));
    highp vec3 specular_color = light.color * light.specular_intensity;
    specular_color *= pow(clamp(dot(view_direction, reflect_direction), 0.0, 1.0), material_shininess);

    /* Final render */
    if (gl_FrontFacing)
        final_color = fragment_color;
    else
        final_color = (vec3(1.f, 1.f, 1.f) - fragment_color);
    final_color *= ambient_color + diffuse_color + specular_color;
}

