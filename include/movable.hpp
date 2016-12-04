#ifndef MOVABLE_HPP
#define MOVABLE_HPP

#include "glm/vec3.hpp" /* glm::vec3 */

class Movable
{
    glm::vec3 _position;
    glm::vec3 _scale;
    vec4f _rotation;

    public:
    Movable();
    Movable(glm::vec3 position, glm::vec3 scale = glm::vec3(1.f, 1.f, 1.f));
    ~Movable();

    void set_position(const Mesh &other);
    void set_position(glm::vec3 position);
    void translate(glm::vec3 position);
    void translate_rel(glm::vec3 position);
    void set_rotation(const Mesh &other);
    void set_rotation(float angle, glm::vec3 axis);
    void rotate(float angle, glm::vec3 axis);
    void set_scale(const Mesh &other);
    void set_scale(const float factor);
    void set_scale(glm::vec3 factor);
    void resize(float factor);
    void resize(glm::vec3 factor);
};

#endif // MOVABLE_HPP
