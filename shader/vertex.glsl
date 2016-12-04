#version 300 es

in vec3 vertex_position;
in vec3 vertex_color;
in vec2 vertex_texture;
in vec3 vertex_normal;

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

//out vec4 gl_Position;
smooth out highp vec3 fragment_position;
smooth out highp vec3 fragment_color;
smooth out highp vec2 fragment_texture;
smooth out highp vec3 fragment_normal;


void main()
{
    gl_Position = model_view_projection_matrix * vec4(vertex_position, 1.0);

    fragment_position = (model_matrix * vec4(vertex_position, 1.0)).xyz;
    fragment_color = vertex_color;
    fragment_texture = vertex_texture;
    fragment_normal = (model_matrix * vec4(vertex_normal, 0.0)).xyz;
}

