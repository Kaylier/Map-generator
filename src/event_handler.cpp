#include "event_handler.hpp"

Event_handler::Event_handler() : 
    _speed_slow(0.1f), 
    _speed_normal(1.f), 
    _speed_fast(10.f), 
    _sensibility(0.005f), 
    _camera(nullptr)
{
}

Event_handler::Event_handler(Camera *camera) : 
    _speed_slow(0.1f), 
    _speed_normal(1.f), 
    _speed_fast(10.f), 
    _sensibility(0.005f), 
    _camera(camera)
{
}

void Event_handler::bind_camera(Camera *camera)
{
    _camera = camera;
}

bool Event_handler::run(sf::Window &window)
{
    sf::Event event;
    float ratio;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                return false;
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                    return false;
                break;
            case sf::Event::Resized:
                ratio = (float)(window.getSize().x) / (float)(window.getSize().y);
                _camera->perspective(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);
                break;
            default:
                break;
        }
    }
    float speed = _speed_normal;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
        speed = _speed_fast;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        speed = _speed_slow;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        _camera->translate_rel(glm::vec3(0.f, 0.f, -speed));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        _camera->translate_rel(glm::vec3(-speed, 0.f, 0.f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        _camera->translate_rel(glm::vec3(0.f, 0.f, speed));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        _camera->translate_rel(glm::vec3(speed, 0.f, 0.f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        _camera->translate(glm::vec3(0.f, 0.f, speed));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        _camera->translate(glm::vec3(0.f, 0.f, -speed));

    float dx, dy;
    dx = (float)(sf::Mouse::getPosition(window).x - (int)(window.getSize().x/2)) * _sensibility;
    dy = (float)(sf::Mouse::getPosition(window).y - (int)(window.getSize().y)/2) * _sensibility;
    _camera->rotate(dx, dy);
    sf::Mouse::setPosition(sf::Vector2i((int)window.getSize().x/2, (int)window.getSize().y/2), window);
    return true;
}

