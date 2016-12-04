#include <iostream>

#include "glm/vec3.hpp" /* glm::vec3 */

#include <SFML/Graphics.hpp>

#include "graphic_engine.hpp"
#include "event_handler.hpp"

int main()
{
    /***************************/
    /* Create a window context */
    /***************************/
    sf::ContextSettings context_settings;
    context_settings.depthBits = 24;
    context_settings.stencilBits = 8;
    context_settings.antialiasingLevel = 2;

    sf::RenderWindow window(sf::VideoMode(1024, 768), 
                            "3d-engine", 
                            sf::Style::None, 
                            //sf::Style::Fullscreen, 
                            context_settings);
    window.setVerticalSyncEnabled(true);
    window.setActive();

    window.setMouseCursorVisible(false);

    /*******************/
    /* Initialize GLEW */
    /*******************/
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        critical_error("%s", "Failed to initialize GLEW");
    note("OpenGL vendor %s", glGetString(GL_VENDOR));
    note("OpenGL renderer %s", glGetString(GL_RENDERER));
    note("OpenGL version %s", glGetString(GL_VERSION));
    note("GLSL version : %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    /***************************/
    /* Create a graphic engine */
    /***************************/
    Graphic_engine graphic_engine;
    graphic_engine.set_background_color(0.2f, 0.5f, 0.9f);
    graphic_engine.set_axis();
    graphic_engine.set_grid();

    float ratio = (float)(window.getSize().x) / (float)window.getSize().y;
    graphic_engine.camera().perspective(-ratio, ratio, 
                                        -1.f, 1.f, 
                                        1.0f, 500.f);
    graphic_engine.camera().set_position(glm::vec3(16.f, 16.f, 24.f));
    graphic_engine.camera().set_position(glm::vec3(5.f, 5.f, 5.f));
    graphic_engine.camera().look_at(glm::vec3(0.f, 0.f, 0.f));

    Event_handler event_handler(&graphic_engine.camera());

    /*************/
    /* Main loop */
    /*************/
    while (event_handler.run(window))
    {
        //window.pushGLStates();
        //window.draw(back);
        //window.popGLStates();

        graphic_engine.update_and_draw();

        //window.pushGLStates();
        //window.draw(front);
        //window.popGLStates();

        window.display();
    }

    window.close();
    return EXIT_SUCCESS;
}

