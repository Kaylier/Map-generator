#include "graphic_engine.hpp"

Graphic_engine::Graphic_engine() : 
    _ubo_precomputed_id(0), 
    _camera(),
    _axis(nullptr), 
    _grid(nullptr), 
    _map()
{
    /********************/
    /* Configure OpenGL */
    /********************/
    note("%s", "Configure OpenGL...");
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.f);
    
    //glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glClearColor(0.f, 0.f, 0.f, 1.0f);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    glLineWidth(1.0);

    /**********************/
    /* Initialize shaders */
    /**********************/
    note("%s", "Initialize shaders...");
    _shader_program[Shader_program::SHADER_PROGRAM_BASIC].load_shader("shader/vertex.glsl", GL_VERTEX_SHADER);
    _shader_program[Shader_program::SHADER_PROGRAM_BASIC].load_shader("shader/fragment.glsl", GL_FRAGMENT_SHADER);
    _shader_program[Shader_program::SHADER_PROGRAM_BASIC].link_shaders();

    /************************************/
    /* Initialize Uniform Buffer Object */
    /************************************/
    glGenBuffers(1, &_ubo_precomputed_id);
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo_precomputed_id); {
        glBufferData(GL_UNIFORM_BUFFER, 
                     _ubo_precomputed_size[NB_SHADER_UNIFORM_PRECOMPUTED], 
                     nullptr, GL_DYNAMIC_DRAW);
    } glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Graphic_engine::~Graphic_engine()
{
    delete _axis;
    delete _grid;
    for (int shd_prg = 0 ; shd_prg < Shader_program::NB_SHADER_PROGRAM ; ++shd_prg) {
        for (std::vector<Drawable*>::iterator it = _drawable[shd_prg].begin() ; 
                it != _drawable[shd_prg].end() ; ++it) {
            delete *it;
        }
    }
    glDeleteBuffers(1, &_ubo_precomputed_id);
}

Camera &Graphic_engine::camera()
{
    return _camera;
}

void Graphic_engine::set_background_color(glm::vec3 color)
{
    glClearColor(color.r, color.g, color.b, 1.0f);
}

void Graphic_engine::set_background_color(glm::vec4 color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void Graphic_engine::set_background_color(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void Graphic_engine::set_axis(bool state)
{
    if (_axis == nullptr && state == true) {
        note("%s", "Show axis");
        float data[6][11] = {
            {-8.f, 0.f, 0.f,   1.f, 0.f, 0.f,   0.f, 0.f,   0.f, 1.f, 0.f}, 
            {16.f, 0.f, 0.f,   1.f, 0.f, 0.f,   0.f, 0.f,   0.f, 1.f, 0.f}, 
            {0.f, -8.f, 0.f,   0.f, 1.f, 0.f,   0.f, 0.f,   0.f, 0.f, 1.f}, 
            {0.f, 16.f, 0.f,   0.f, 1.f, 0.f,   0.f, 0.f,   0.f, 0.f, 1.f}, 
            {0.f, 0.f, -8.f,   0.f, 0.f, 1.f,   0.f, 0.f,   1.f, 0.f, 0.f}, 
            {0.f, 0.f, 16.f,   0.f, 0.f, 1.f,   0.f, 0.f,   1.f, 0.f, 0.f}
        };
        uint32_t elements[6] = {0, 1, 2, 3, 4, 5};
        _axis = new Drawable(data, 6, elements, 6, GL_LINES, GL_STATIC_DRAW);
    } else if (_axis != nullptr && state == false) {
        note("%s", "Hide axis");
        delete _axis;
        _axis = nullptr;
    }
}

void Graphic_engine::toggle_axis()
{
    set_axis(_axis == nullptr);
}

void Graphic_engine::set_grid(bool state)
{
    if (_grid == nullptr && state == true) {
        note("%s", "Show grid");
        float positions[32*4*3];
        for (size_t i = 0 ; i+11 < 32*4*3 ; i += 12) {
            positions[i]    = -512.f + (float)(i*32/12);
            positions[i+1]  = -512.f;
            positions[i+2]  = 0.f;
            positions[i+3]  = -512.f + (float)(i*32/12);
            positions[i+4]  = 512.f;
            positions[i+5]  = 0.f;
            positions[i+6]  = -512.f;
            positions[i+7]  = -512.f + (float)(i*32/12);
            positions[i+8]  = 0.f;
            positions[i+9]  = 512.f;
            positions[i+10] = -512.f + (float)(i*32/12);
            positions[i+11] = 0.f;
        }
        float colors[32*4*3];
        for (size_t i = 0 ; i+11 < 32*4*3 ; i += 12) {
            colors[i]    = 0.f;
            colors[i+1]  = 1.f;
            colors[i+2]  = 0.f;
            colors[i+3]  = 0.f;
            colors[i+4]  = 1.f;
            colors[i+5]  = 0.f;
            colors[i+6]  = 1.f;
            colors[i+7]  = 0.f;
            colors[i+8]  = 0.f;
            colors[i+9]  = 1.f;
            colors[i+10] = 0.f;
            colors[i+11] = 0.f;
        }
        float textures[32*4*2];
        float normals[32*4*3];
        for (size_t i = 0 ; i+11 < 32*4*3 ; i += 12) {
            normals[i]    = 0.f;
            normals[i+1]  = 0.f;
            normals[i+2]  = 1.f;
            normals[i+3]  = 0.f;
            normals[i+4]  = 0.f;
            normals[i+5]  = 1.f;
            normals[i+6]  = 0.f;
            normals[i+7]  = 0.f;
            normals[i+8]  = 1.f;
            normals[i+9]  = 0.f;
            normals[i+10] = 0.f;
            normals[i+11] = 1.f;
        }
        const float *data[Shader_program::NB_SHADER_ATTRIB] = {positions, colors, textures, normals};
        uint32_t elements[32*4];
        for (uint32_t i = 0 ; i < 32*4 ; ++i)
            elements[i] = i;
        _grid = new Drawable(data, 32*4, elements, 32*4, GL_LINES, GL_STATIC_DRAW);
    } else if (_grid != nullptr && state == false) {
        note("%s", "Hide grid");
        delete _grid;
        _grid = nullptr;
    }
}

void Graphic_engine::toggle_grid()
{
    set_grid(_grid == nullptr);
}

void Graphic_engine::set_wireframe(bool state)
{
    note("%set wireframe mode", (state) ? "S" : "Uns");
    if (state) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Graphic_engine::set_cullface(bool state)
{
    note("%set cullface", (state) ? "S" : "Uns");
    if (state) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

void Graphic_engine::update_and_draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindBufferBase(GL_UNIFORM_BUFFER, Shader_program::SHADER_UNIFORM_BLOCK_PRECOMPUTED, _ubo_precomputed_id);

    /* Update map */
    _map.update(_camera.position());

    /* Update camera */
    glm::mat4 model_matrix, view_matrix, projection_matrix;
    _camera.update_uniforms(view_matrix, projection_matrix);

    /* Draw axis */
    if (_axis) {
        _shader_program[Shader_program::SHADER_PROGRAM_BASIC].use();
        _axis->update_uniforms(model_matrix);
        update_uniforms(model_matrix, view_matrix, projection_matrix);
        _axis->draw();
    }

    /* Draw grid */
    if (_grid) {
        _shader_program[Shader_program::SHADER_PROGRAM_BASIC].use();
        _grid->update_uniforms(model_matrix);
        update_uniforms(model_matrix, view_matrix, projection_matrix);
        _grid->draw();
    }

    /* Draw map */
    _shader_program[Shader_program::SHADER_PROGRAM_BASIC].use();
    do {
    _map.update_uniforms(model_matrix);
    update_uniforms(model_matrix, view_matrix, projection_matrix);
    } while (_map.draw());

    /* Draw others drawables */
    for (int shd_prg = 0 ; shd_prg < Shader_program::NB_SHADER_PROGRAM ; ++shd_prg) {
        _shader_program[shd_prg].use();
        for (std::vector<Drawable*>::iterator it = _drawable[shd_prg].begin() ; 
                it != _drawable[shd_prg].end() ; ++it) {
            (*it)->update_uniforms(model_matrix);
            update_uniforms(model_matrix, view_matrix, projection_matrix);
            (*it)->draw();
        }
    }
    glUseProgram(0);
}

void Graphic_engine::update_uniforms(const glm::mat4 &model_matrix, 
                                     const glm::mat4 &view_matrix, 
                                     const glm::mat4 &projection_matrix) const
{
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo_precomputed_id); {

        glm::mat4 model_view_matrix = view_matrix * model_matrix;
        glBufferSubData(GL_UNIFORM_BUFFER, 
                        _ubo_precomputed_offset[SHADER_UNIFORM_MODEL_VIEW_MATRIX], 
                        _ubo_precomputed_size[SHADER_UNIFORM_MODEL_VIEW_MATRIX], 
                        glm::value_ptr(model_view_matrix));

        glm::mat4 model_view_projection_matrix = projection_matrix * model_view_matrix;
        glBufferSubData(GL_UNIFORM_BUFFER, 
                        _ubo_precomputed_offset[SHADER_UNIFORM_MODEL_VIEW_PROJECTION_MATRIX], 
                        _ubo_precomputed_size[SHADER_UNIFORM_MODEL_VIEW_PROJECTION_MATRIX], 
                        glm::value_ptr(model_view_projection_matrix));

    } glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

