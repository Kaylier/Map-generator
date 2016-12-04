#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include <cstdint> /* int32_t, uint32_t */
#include "glm/mat4x4.hpp" /* glm::mat4 */
#include "glm/gtc/type_ptr.hpp" /* glm::value_ptr */

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "shader_program.hpp"

class Drawable
{
public:
    Drawable() = delete;
    Drawable(const Drawable &drawable);
    Drawable(const float *const data[Shader_program::NB_SHADER_ATTRIB], int32_t vertex_count, 
             const uint32_t elements[], int32_t element_count, 
             GLenum drawing_mode = GL_TRIANGLES, 
             GLenum usage = GL_DYNAMIC_DRAW);
    Drawable(const float (*const data)[11], int32_t vertex_count, 
             const uint32_t elements[], int32_t element_count, 
             GLenum drawing_mode = GL_TRIANGLES, 
             GLenum usage = GL_DYNAMIC_DRAW);
    Drawable(const char *filename);
    virtual ~Drawable();

    Drawable &operator=(const Drawable& drawable) = delete;

    void hide();
    void show();
    bool hidden() const;

    virtual void update_uniforms(glm::mat4 &model_matrix) const;
    virtual void draw() const;

protected:
    enum Shader_uniform_mesh {
        SHADER_UNIFORM_MODEL_MATRIX = 0, 
        NB_SHADER_UNIFORM_MESH
    };
    static constexpr int32_t _ubo_mesh_offset[NB_SHADER_UNIFORM_MESH] = {
        0
    };
    static constexpr int32_t _ubo_mesh_size[NB_SHADER_UNIFORM_MESH+1] = {
        sizeof(glm::mat4), 
        sizeof(glm::mat4) /* total */
    };

    bool _visible;
    GLenum _drawing_mode;

    GLuint _vao_id;
    
    GLuint _vbo_id;
    int32_t _vertex_buffer_offset[Shader_program::NB_SHADER_ATTRIB];
    uint32_t _vertex_buffer_stride;
    
    GLuint _ebo_id;
    int32_t _element_count;
    
    GLuint _ubo_mesh_id;

private:

};

#endif // DRAWABLE_HPP
