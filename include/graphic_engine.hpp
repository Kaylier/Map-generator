#ifndef GRAPHIC_ENGINE_HPP
#define GRAPHIC_ENGINE_HPP

#include <cstdint> /* uint32_t */
#include "glm/vec4.hpp" /* glm::vec4 */
#include "glm/mat4x4.hpp" /* glm::mat4 */
#include "glm/gtc/type_ptr.hpp" /* glm::value_ptr */

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "shader_program.hpp"
#include "camera.hpp"
#include "drawable.hpp"
#include "map.hpp"

#include "debug.hpp"

class Graphic_engine
{
public:
    Graphic_engine();
    Graphic_engine(const Graphic_engine&) = delete;
    virtual ~Graphic_engine();

    Graphic_engine &operator=(const Graphic_engine&) = delete;

    Camera &camera();

    void set_background_color(glm::vec3 color);
    void set_background_color(glm::vec4 color);
    void set_background_color(float r, float g, float b, float a = 1.f);
    void set_axis(bool state = true);
    void toggle_axis();
    void set_grid(bool state = true);
    void toggle_grid();
    void set_wireframe(bool state = true);
    void set_cullface(bool state = true);
    /* TODO: drawing options */

    virtual void update_and_draw();

protected:
    enum Shader_uniform_precomputed {
        SHADER_UNIFORM_MODEL_VIEW_MATRIX = 0, 
        SHADER_UNIFORM_MODEL_VIEW_PROJECTION_MATRIX, 
        NB_SHADER_UNIFORM_PRECOMPUTED
    };
    static constexpr int32_t _ubo_precomputed_offset[NB_SHADER_UNIFORM_PRECOMPUTED] = {
        0, 
        sizeof(glm::mat4)
    };
    static constexpr int32_t _ubo_precomputed_size[NB_SHADER_UNIFORM_PRECOMPUTED+1] = {
        sizeof(glm::mat4), 
        sizeof(glm::mat4), 
        2*sizeof(glm::mat4) /* total */
    };

    Shader_program _shader_program[Shader_program::NB_SHADER_PROGRAM];
    GLuint _ubo_precomputed_id;

    Camera _camera;
    Drawable *_axis, *_grid;
    Map _map;
    std::vector<Drawable*> _drawable[Shader_program::NB_SHADER_PROGRAM];

    void update_uniforms(const glm::mat4 &model_matrix, 
                         const glm::mat4 &view_matrix, 
                         const glm::mat4 &projection_matrix) const;

private: 

};

#endif // GRAPHIC_ENGINE_HPP
