#include "camera.hpp"

Camera::Camera() : 
    _ubo_camera_id(0), 
    _position(0.f, 0.f, 0.f), 
    _theta(0), 
    _phi(0), 
    _projection_matrix(glm::mat4())
{
    glGenBuffers(1, &_ubo_camera_id);
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo_camera_id); {
        glBufferData(GL_UNIFORM_BUFFER, 
                     _ubo_camera_size[NB_SHADER_UNIFORM_CAMERA], 
                     nullptr, GL_DYNAMIC_DRAW);
    } glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, Shader_program::SHADER_UNIFORM_BLOCK_CAMERA, _ubo_camera_id);
}

Camera::Camera(const Camera &other) : 
    _ubo_camera_id(0), 
    _position(other._position),
    _theta(other._theta), 
    _phi(other._phi), 
    _projection_matrix(other._projection_matrix)
{
    glGenBuffers(1, &_ubo_camera_id);
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo_camera_id); {
        glBufferData(GL_UNIFORM_BUFFER, 
                     _ubo_camera_size[NB_SHADER_UNIFORM_CAMERA], 
                     nullptr, GL_DYNAMIC_DRAW);
    } glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, Shader_program::SHADER_UNIFORM_BLOCK_CAMERA, _ubo_camera_id);
}

Camera::~Camera()
{
    glDeleteBuffers(1, &_ubo_camera_id);
}

Camera &Camera::operator=(const Camera &other)
{
    _position = other._position;
    _theta = other._theta;
    _phi = other._phi;
    _projection_matrix = other._projection_matrix;
    return *this;
}

glm::vec3 Camera::position() const
{
    return _position;
}

void Camera::set_position(glm::vec3 pos)
{
    _position = pos;
}

void Camera::translate(glm::vec3 trans)
{
    _position += trans;
}

void Camera::translate_rel(glm::vec3 trans)
{
    glm::mat4 view_matrix = glm::mat4();
    view_matrix = glm::rotate(view_matrix, 1.57f, glm::vec3(1.f, 0.f, 0.f));
    view_matrix = glm::rotate(view_matrix, -_theta, glm::vec3(0.f, 1.f, 0.f));
    view_matrix = glm::rotate(view_matrix, -_phi, glm::vec3(1.f, 0.f, 0.f));

    glm::vec4 dir = view_matrix * glm::vec4(trans, 0.f);
    _position.x += dir.x;
    _position.y += dir.y;
    _position.z += dir.z;
}

void Camera::set_rotation(float angle_theta, float angle_phi)
{
    _theta = angle_theta;
    _phi = angle_phi;
    if (_phi > 1.57f)
        _phi = 1.57f;
    else if (_phi < -1.57f)
        _phi = -1.57f;
}

void Camera::rotate(float angle_theta, float angle_phi)
{
    _theta += angle_theta;
    _phi += angle_phi;
    if (_phi > 1.57f)
        _phi = 1.57f;
    else if (_phi < -1.57f)
        _phi = -1.57f;
}

void Camera::look_at(glm::vec3 pos)
{
    _theta = (float)atan2(pos.x - _position.x, pos.y - _position.y);;
    float r;
    r  = (pos.x - _position.x)*(pos.x - _position.x);
    r += (pos.y - _position.y)*(pos.y - _position.y);
    _phi = (float)-atan2(pos.z - _position.z, sqrt(r));
}

void Camera::orthographic(float left, float right, 
                          float bottom, float top, 
                          float near_val, float far_val)
{
    _projection_matrix = glm::mat4(
            2.f/(right-left), 0.f, 0.f, -(right+left)/(right-left), 
            0.f, 2.f/(top-bottom), 0.f, -(top+bottom)/(top-bottom), 
            0.f, 0.f, -2.f/(far_val-near_val), -(far_val+near_val)/(far_val-near_val), 
            0.f, 0.f, 0.f, 1.f);
}

void Camera::perspective(float left, float right, 
                         float bottom, float top, 
                         float near_val, float far_val)
{
    _projection_matrix = glm::mat4(
            2.f*near_val/(right-left), 0.f, 0.f, 0.f, 
            0.f, 2.f*near_val/(top-bottom), 0.f, 0.f, 
            (right+left)/(right-left), (top+bottom)/(top-bottom), -(far_val+near_val)/(far_val-near_val), -1.f, 
            0.f, 0.f, -2.f*far_val*near_val/(far_val-near_val), 0.f);
}

void Camera::curvilinear_perspective(float left, float right, 
                                     float bottom, float top, 
                                     float near_val, float far_val)
{
    /* TODO: projection curvilinear perspective */
    not_implemented();
    (void)left; (void)right; (void)bottom; (void)top; (void)near_val; (void)far_val;
}

void Camera::reverse_perspective(float left, float right, 
                                 float bottom, float top, 
                                 float near_val, float far_val)
{
    /* TODO: projection reverse perspective */
    not_implemented();
    (void)left; (void)right; (void)bottom; (void)top; (void)near_val; (void)far_val;
}

void Camera::update_uniforms(glm::mat4 &view_matrix, 
                             glm::mat4 &projection_matrix) const
{
    /* TODO: optimize rotations multiplication into one
     * selon x : cos(phi) = sqrt(x*x+z*z), sin(phi) = y
     * selon y : cos(theta) = -z/cos(phi), sin(theta) = 
     * 
     * (1.f, 0.f, 0.f, 0.f)
     * (0.f, c  , -s , 0.f)
     * (0.f, s  , c  , 0.f)
     * (0.f, 0.f, 0.f, 1.f)
     */
    /*view_matrix = Mat4f::rotation(_phi, 1.f, 0.f, 0.f);
    view_matrix *= Mat4f::rotation(_theta, 0.f, 1.f, 0.f);
    view_matrix *= Mat4f::rotation(-90.f, 1.f, 0.f, 0.f);
    view_matrix *= Mat4f(1.f, 0.f, 0.f,-_position.x, 
                  0.f, 1.f, 0.f,-_position.y, 
                  0.f, 0.f, 1.f,-_position.z, 
                  0.f, 0.f, 0.f, 1.f);*/
    view_matrix = glm::mat4();
    view_matrix = glm::rotate(view_matrix, _phi, glm::vec3(1.f, 0.f, 0.f));
    view_matrix = glm::rotate(view_matrix, _theta, glm::vec3(0.f, 1.f, 0.f));
    view_matrix = glm::rotate(view_matrix, -1.57f, glm::vec3(1.f, 0.f, 0.f));
    view_matrix = glm::translate(view_matrix, -_position);

    projection_matrix = _projection_matrix;

    glBindBuffer(GL_UNIFORM_BUFFER, _ubo_camera_id); {

        glBufferSubData(GL_UNIFORM_BUFFER, 
                        (size_t)_ubo_camera_offset[SHADER_UNIFORM_VIEW_MATRIX], 
                        (size_t)_ubo_camera_size[SHADER_UNIFORM_VIEW_MATRIX], 
                        glm::value_ptr(view_matrix));

        glBufferSubData(GL_UNIFORM_BUFFER, 
                        (size_t)_ubo_camera_offset[SHADER_UNIFORM_PROJECTION_MATRIX], 
                        (size_t)_ubo_camera_size[SHADER_UNIFORM_PROJECTION_MATRIX], 
                        glm::value_ptr(_projection_matrix));

        glBufferSubData(GL_UNIFORM_BUFFER, 
                        (size_t)_ubo_camera_offset[SHADER_UNIFORM_VIEW_POSITION], 
                        (size_t)_ubo_camera_size[SHADER_UNIFORM_VIEW_POSITION], 
                        glm::value_ptr(_position));

    } glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

