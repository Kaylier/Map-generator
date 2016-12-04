#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/vec3.hpp" /* glm::vec3, glm::vec4 */
#include "glm/mat4x4.hpp" /* glm::mat4 */
#include "glm/gtc/matrix_transform.hpp" /* glm::rotate, glm::translate */
#include "glm/gtc/type_ptr.hpp" /* glm::value_ptr */

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "shader_program.hpp"

class Camera
{
public:
    Camera();
    Camera(const Camera &other);
    virtual ~Camera();

    Camera &operator=(const Camera&);

    /* View */
    glm::vec3 position() const;
    void set_position(glm::vec3 pos);
    void translate(glm::vec3 trans);
    void translate_rel(glm::vec3 trans);
    void set_rotation(float angle_theta, float angle_phi);
    void rotate(float angle_theta, float angle_phi);
    void look_at(glm::vec3 pos);

    /* Projection */
    void orthographic(float left, float right, 
                      float bottom, float top, 
                      float near_val, float far_val);
    void perspective(float left, float right, 
                     float bottom, float top, 
                     float near_val, float far_val);
    void curvilinear_perspective(float left, float right, 
                                 float bottom, float top, 
                                 float near_val, float far_val);
    void reverse_perspective(float left, float right, 
                             float bottom, float top, 
                             float near_val, float far_val);

    void update_uniforms(glm::mat4 &view_matrix, 
                         glm::mat4 &projection_matrix) const;


protected:
    enum Shader_uniform_camera {
        SHADER_UNIFORM_VIEW_MATRIX = 0, 
        SHADER_UNIFORM_PROJECTION_MATRIX, 
        SHADER_UNIFORM_VIEW_POSITION, 
        NB_SHADER_UNIFORM_CAMERA
    };
    static constexpr int32_t _ubo_camera_offset[NB_SHADER_UNIFORM_CAMERA] = {
        0, 
        sizeof(glm::mat4), 
        2*sizeof(glm::mat4)
    };
    static constexpr int32_t _ubo_camera_size[NB_SHADER_UNIFORM_CAMERA+1] = {
        sizeof(glm::mat4), 
        sizeof(glm::mat4), 
        sizeof(glm::vec3), 
        2*sizeof(glm::mat4) + sizeof(glm::vec3) /* total */
    };

    GLuint _ubo_camera_id;

    glm::vec3 _position;
    float _theta, _phi;
    glm::mat4 _projection_matrix;

private:

};

#endif // CAMERA_HPP
