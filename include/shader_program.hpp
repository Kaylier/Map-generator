#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include <string>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "debug.hpp"

class Shader_program
{
public:
    enum Program
    {
        SHADER_PROGRAM_BASIC = 0, 
        NB_SHADER_PROGRAM
    };
    enum Attribute
    {
        SHADER_ATTRIB_POSITION = 0, 
        SHADER_ATTRIB_COLOR, 
        SHADER_ATTRIB_TEXTURE, 
        SHADER_ATTRIB_NORMAL, 
        NB_SHADER_ATTRIB
    };
    enum Uniform_block
    {
        SHADER_UNIFORM_BLOCK_CAMERA = 0, 
        SHADER_UNIFORM_BLOCK_MESH, 
        SHADER_UNIFORM_BLOCK_PRECOMPUTED, 
        SHADER_UNIFORM_BLOCK_LIGHT, 
        SHADER_UNIFORM_BLOCK_MATERIAL, 
        NB_SHADER_UNIFORM
    };

    Shader_program();
    Shader_program(const Shader_program&) = delete;
    virtual ~Shader_program();

    Shader_program &operator=(const Shader_program&) = delete;

    int load_shader(const char *filename, GLenum shader_type);
    int load_shader_source(const char *source, GLenum shader_type);
    int link_shaders();
    void use() const;

    void print_summary() const;

protected:
    std::vector<GLuint> _shaders;
    GLuint _program_id;

    inline void bind_attrib(const char *name, enum Attribute attrib);
    inline void bind_uniform_block(const char *name, enum Uniform_block block);

private:

};

#endif // SHADER_PROGRAM_HPP
