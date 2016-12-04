#include "map_chunk.hpp"

Map_chunk::Map_chunk(const struct Args &arg) : 
    Drawable(arg.data, arg.vertex_count, 
             arg.elements, arg.element_count, 
             GL_TRIANGLES, GL_STATIC_DRAW), 
    _position(arg.position), 
    _quality(arg.quality)
{
}

Map_chunk::Map_chunk(glm::ivec3 position, uint8_t quality, 
        const float *const data[Shader_program::NB_SHADER_ATTRIB], int32_t vertex_count, 
        const uint32_t elements[], int32_t element_count) : 
    Drawable(data, vertex_count, 
             elements, element_count, 
             GL_TRIANGLES, GL_STATIC_DRAW), 
    _position(position), 
    _quality(quality)
{
}

Map_chunk::Map_chunk(glm::ivec3 position, uint8_t quality, 
        const float (*const data)[11], int32_t vertex_count, 
        const uint32_t elements[], int32_t element_count) : 
    Drawable(data, vertex_count, 
             elements, element_count, 
             GL_TRIANGLES, GL_STATIC_DRAW), 
    _position(position), 
    _quality(quality)
{
}

glm::ivec3 Map_chunk::position() const
{
    return _position;
}

uint8_t Map_chunk::quality() const
{
    return _quality;
}


void Map_chunk::update_uniforms(glm::mat4 &model_matrix) const
{
    model_matrix = glm::translate(glm::mat4(), glm::vec3(_position));

    glBindBuffer(GL_UNIFORM_BUFFER, _ubo_mesh_id); {

        glBufferSubData(GL_UNIFORM_BUFFER, 
                        _ubo_mesh_offset[SHADER_UNIFORM_MODEL_MATRIX], 
                        _ubo_mesh_size[SHADER_UNIFORM_MODEL_MATRIX], 
                        glm::value_ptr(model_matrix));

    } glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Map_chunk::print_summary(const struct Args &arg)
{
    note("Summary of the map chunk (%i, %i, %i) with quality [%u]", arg.position.x, arg.position.y, arg.position.z, arg.quality);
    note("Vertices : %i", arg.vertex_count);
    for (int i = 0 ; i < arg.vertex_count ; ++i)
        note(" #%i (%1.1f, %1.1f, %1.1f) (%1.1f, %1.1f, %1.1f) (%1.1f, %1.1f) (%1.1f, %1.1f, %1.1f)", i, 
                (double)arg.data[0][i*3], (double)arg.data[0][i*3+1], (double)arg.data[0][i*3+2], 
                (double)arg.data[1][i*3], (double)arg.data[1][i*3+1], (double)arg.data[1][i*3+2], 
                (double)arg.data[2][i*3], (double)arg.data[2][i*3+1], 
                (double)arg.data[3][i*3], (double)arg.data[3][i*3+1], (double)arg.data[3][i*3+2]);
    note("Elements : %i", arg.element_count);
    for (int i = 0 ; i < arg.element_count ; ++i)
        note(" #%i %u", i, arg.elements[i]);
}

