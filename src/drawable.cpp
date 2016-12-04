#include "drawable.hpp"

Drawable::Drawable(const Drawable &drawable)
{
    /* TODO: Copy constructor */
    not_implemented();
    (void)drawable;
}

Drawable::Drawable(const float *const data[Shader_program::NB_SHADER_ATTRIB], int32_t vertex_count, 
                   const uint32_t elements[], int32_t element_count, 
                   GLenum drawing_mode, 
                   GLenum usage) : 
    _visible(true), 
    _drawing_mode(drawing_mode), 
    _vao_id(0), 
    _vbo_id(0), 
    _ebo_id(0), 
    _ubo_mesh_id(0)
{
    /*********************************/
    /* Pre-compute offsets and sizes */
    /*********************************/
    _vertex_buffer_offset[Shader_program::SHADER_ATTRIB_POSITION] = vertex_count * 0 * (int32_t)sizeof(float);
    _vertex_buffer_offset[Shader_program::SHADER_ATTRIB_COLOR]    = vertex_count * 3 * (int32_t)sizeof(float);
    _vertex_buffer_offset[Shader_program::SHADER_ATTRIB_TEXTURE]  = vertex_count * 6 * (int32_t)sizeof(float);
    _vertex_buffer_offset[Shader_program::SHADER_ATTRIB_NORMAL]   = vertex_count * 8 * (int32_t)sizeof(float);

    _vertex_buffer_stride = 0;

    _element_count = element_count;

    /***********************************/
    /* Initialize Vertex Buffer Object */
    /***********************************/
    glGenBuffers(1, &_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_id); {
        glBufferData(GL_ARRAY_BUFFER, vertex_count * 11 * (int32_t)sizeof(float), nullptr, usage);
        glBufferSubData(GL_ARRAY_BUFFER, 
                        _vertex_buffer_offset[Shader_program::SHADER_ATTRIB_POSITION], 
                        vertex_count * 3 * (int32_t)sizeof(float), 
                        data[Shader_program::SHADER_ATTRIB_POSITION]);
        glBufferSubData(GL_ARRAY_BUFFER, 
                        _vertex_buffer_offset[Shader_program::SHADER_ATTRIB_COLOR], 
                        vertex_count * 3 * (int32_t)sizeof(float), 
                        data[Shader_program::SHADER_ATTRIB_COLOR]);
        glBufferSubData(GL_ARRAY_BUFFER, 
                        _vertex_buffer_offset[Shader_program::SHADER_ATTRIB_TEXTURE], 
                        vertex_count * 2 * (int32_t)sizeof(float), 
                        data[Shader_program::SHADER_ATTRIB_TEXTURE]);
        glBufferSubData(GL_ARRAY_BUFFER, 
                        _vertex_buffer_offset[Shader_program::SHADER_ATTRIB_NORMAL], 
                        vertex_count * 3 * (int32_t)sizeof(float), 
                        data[Shader_program::SHADER_ATTRIB_NORMAL]);
    } glBindBuffer(GL_ARRAY_BUFFER, 0);

    /************************************/
    /* Initialize Element Buffer Object */
    /************************************/
    glGenBuffers(1, &_ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo_id); {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                     element_count * (int32_t)sizeof(uint32_t), 
                     elements, 
                     usage);
    } glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /************************************/
    /* Initialize Uniform Buffer Object */
    /************************************/
    glGenBuffers(1, &_ubo_mesh_id);
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo_mesh_id); {
        glBufferData(GL_UNIFORM_BUFFER, 
                     _ubo_mesh_size[NB_SHADER_UNIFORM_MESH], 
                     nullptr, 
                     usage);
    } glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, Shader_program::SHADER_UNIFORM_BLOCK_MESH, _ubo_mesh_id);

    /**********************************/
    /* Initialize Array Buffer Object */
    /**********************************/
    glGenVertexArrays(1, &_vao_id);
    glBindVertexArray(_vao_id); {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_id);

        glEnableVertexAttribArray(Shader_program::SHADER_ATTRIB_POSITION);
        glVertexAttribPointer(Shader_program::SHADER_ATTRIB_POSITION, 
                3, GL_FLOAT, GL_FALSE, 
                _vertex_buffer_stride, 
                (void*)_vertex_buffer_offset[Shader_program::SHADER_ATTRIB_POSITION]);

        glEnableVertexAttribArray(Shader_program::SHADER_ATTRIB_COLOR);
        glVertexAttribPointer(Shader_program::SHADER_ATTRIB_COLOR, 
                3, GL_FLOAT, GL_FALSE, 
                _vertex_buffer_stride, 
                (void*)_vertex_buffer_offset[Shader_program::SHADER_ATTRIB_COLOR]);

        glEnableVertexAttribArray(Shader_program::SHADER_ATTRIB_TEXTURE);
        glVertexAttribPointer(Shader_program::SHADER_ATTRIB_TEXTURE, 
                2, GL_FLOAT, GL_FALSE, 
                _vertex_buffer_stride, 
                (void*)_vertex_buffer_offset[Shader_program::SHADER_ATTRIB_TEXTURE]);

        glEnableVertexAttribArray(Shader_program::SHADER_ATTRIB_NORMAL);
        glVertexAttribPointer(Shader_program::SHADER_ATTRIB_NORMAL, 
                3, GL_FLOAT, GL_FALSE, 
                _vertex_buffer_stride, 
                (void*)_vertex_buffer_offset[Shader_program::SHADER_ATTRIB_NORMAL]);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo_id);
    } glBindVertexArray(0);
}

Drawable::Drawable(const float (*const data)[11], int32_t vertex_count, 
                   const uint32_t elements[], int32_t element_count, 
                   GLenum drawing_mode, 
                   GLenum usage) : 
    _visible(true), 
    _drawing_mode(drawing_mode), 
    _vao_id(0), 
    _vbo_id(0), 
    _ebo_id(0), 
    _ubo_mesh_id(0)
{
    /*********************************/
    /* Pre-compute offsets and sizes */
    /*********************************/
    _vertex_buffer_offset[Shader_program::SHADER_ATTRIB_POSITION] = 0 * (int32_t)sizeof(float);
    _vertex_buffer_offset[Shader_program::SHADER_ATTRIB_COLOR]    = 3 * (int32_t)sizeof(float);
    _vertex_buffer_offset[Shader_program::SHADER_ATTRIB_TEXTURE]  = 6 * (int32_t)sizeof(float);
    _vertex_buffer_offset[Shader_program::SHADER_ATTRIB_NORMAL]   = 8 * (int32_t)sizeof(float);

    _vertex_buffer_stride = 11 * (int32_t)sizeof(float);

    _element_count = element_count;

    /***********************************/
    /* Initialize Vertex Buffer Object */
    /***********************************/
    glGenBuffers(1, &_vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_id); {
        glBufferData(GL_ARRAY_BUFFER, vertex_count * 11 * (int32_t)sizeof(float), nullptr, usage);
        glBufferSubData(GL_ARRAY_BUFFER, 
                        0, 
                        vertex_count * _vertex_buffer_stride, 
                        data);
    } glBindBuffer(GL_ARRAY_BUFFER, 0);

    /************************************/
    /* Initialize Element Buffer Object */
    /************************************/
    glGenBuffers(1, &_ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo_id); {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                     element_count * (int32_t)sizeof(uint32_t), 
                     elements, 
                     usage);
    } glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /************************************/
    /* Initialize Uniform Buffer Object */
    /************************************/
    glGenBuffers(1, &_ubo_mesh_id);
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo_mesh_id); {
        glBufferData(GL_UNIFORM_BUFFER, 
                     _ubo_mesh_size[NB_SHADER_UNIFORM_MESH], 
                     nullptr, 
                     usage);
    } glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, Shader_program::SHADER_UNIFORM_BLOCK_MESH, _ubo_mesh_id);

    /**********************************/
    /* Initialize Array Buffer Object */
    /**********************************/
    glGenVertexArrays(1, &_vao_id);
    glBindVertexArray(_vao_id); {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo_id);

        glEnableVertexAttribArray(Shader_program::SHADER_ATTRIB_POSITION);
        glVertexAttribPointer(Shader_program::SHADER_ATTRIB_POSITION, 
                3, GL_FLOAT, GL_FALSE, 
                _vertex_buffer_stride, 
                (void*)_vertex_buffer_offset[Shader_program::SHADER_ATTRIB_POSITION]);

        glEnableVertexAttribArray(Shader_program::SHADER_ATTRIB_COLOR);
        glVertexAttribPointer(Shader_program::SHADER_ATTRIB_COLOR, 
                3, GL_FLOAT, GL_FALSE, 
                _vertex_buffer_stride, 
                (void*)_vertex_buffer_offset[Shader_program::SHADER_ATTRIB_COLOR]);

        glEnableVertexAttribArray(Shader_program::SHADER_ATTRIB_TEXTURE);
        glVertexAttribPointer(Shader_program::SHADER_ATTRIB_TEXTURE, 
                2, GL_FLOAT, GL_FALSE, 
                _vertex_buffer_stride, 
                (void*)_vertex_buffer_offset[Shader_program::SHADER_ATTRIB_TEXTURE]);

        glEnableVertexAttribArray(Shader_program::SHADER_ATTRIB_NORMAL);
        glVertexAttribPointer(Shader_program::SHADER_ATTRIB_NORMAL, 
                3, GL_FLOAT, GL_FALSE, 
                _vertex_buffer_stride, 
                (void*)_vertex_buffer_offset[Shader_program::SHADER_ATTRIB_NORMAL]);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo_id);
    } glBindVertexArray(0);
}

Drawable::Drawable(const char *filename)
{
    /* TODO: Construct from a file */
    not_implemented();
    (void)filename;
}

Drawable::~Drawable()
{
    glDeleteBuffers(1, &_vbo_id);
    glDeleteBuffers(1, &_ebo_id);
    glDeleteBuffers(1, &_ubo_mesh_id);
    glDeleteVertexArrays(1, &_vao_id);
}

void Drawable::hide()
{
    _visible = false;
}

void Drawable::show()
{
    _visible = true;
}

bool Drawable::hidden() const
{
    return _visible;
}

void Drawable::update_uniforms(glm::mat4 &model_matrix) const
{
    model_matrix = glm::mat4();

    glBindBuffer(GL_UNIFORM_BUFFER, _ubo_mesh_id); {

        glBufferSubData(GL_UNIFORM_BUFFER, 
                        _ubo_mesh_offset[SHADER_UNIFORM_MODEL_MATRIX], 
                        _ubo_mesh_size[SHADER_UNIFORM_MODEL_MATRIX], 
                        glm::value_ptr(model_matrix));

    } glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Drawable::draw() const
{
    glBindBufferBase(GL_UNIFORM_BUFFER, Shader_program::SHADER_UNIFORM_BLOCK_MESH, _ubo_mesh_id);
    glBindVertexArray(_vao_id); {

        glDrawElements(_drawing_mode, _element_count, GL_UNSIGNED_INT, nullptr);

    } glBindVertexArray(0);
}

