#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP

#include "glm/vec3.hpp" /* glm::vec3 */

#include <SFML/Window.hpp>

#include "camera.hpp"

class Event_handler
{
public:
    Event_handler();
    Event_handler(const Event_handler&) = default;
    Event_handler(Camera *camera);
    virtual ~Event_handler() = default;

    Event_handler &operator=(const Event_handler&) = default;

    void bind_camera(Camera *camera);
    bool run(sf::Window &window);

protected:
    float _speed_slow, _speed_normal, _speed_fast;
    float _sensibility;
    Camera *_camera;

private:

};

#endif // EVENT_HANDLER_HPP
