#include "shader_program.hpp"

Shader_program::Shader_program() : 
    _shaders(), 
    _program_id(0)
{
}

Shader_program::~Shader_program()
{
    note("Delete shader program #%u", _program_id);
    glDeleteProgram(_program_id);
}

int Shader_program::load_shader(const char *filename, GLenum shader_type)
{
    std::string code;
	std::ifstream stream(filename, std::ios::in);
	if(!stream.is_open()) {
        error("Failed to open '%s'", filename);
		return 0;
    }

    note("Read shader file '%s'...", filename);
	std::string Line;
	while(getline(stream, Line))
		code += "\n" + Line;
	stream.close();

    return load_shader_source(code.c_str(), shader_type);
}

int Shader_program::load_shader_source(const char *source, GLenum shader_type)
{
    note("Compile %s shader...", 
            (shader_type == GL_VERTEX_SHADER) ? "vertex" : 
            (shader_type == GL_FRAGMENT_SHADER) ? "fragment" : 
            (shader_type == GL_GEOMETRY_SHADER) ? "geometry" : 
            (shader_type == GL_COMPUTE_SHADER) ? "compute" : 
            (shader_type == GL_TESS_CONTROL_SHADER) ? "tess-control" : 
            (shader_type == GL_TESS_EVALUATION_SHADER) ? "tess-evaluation" : "unknown" );

    GLuint id = glCreateShader(shader_type);
	glShaderSource(id, 1, &source , nullptr);
	glCompileShader(id);
    
    GLint status, log_len;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_len);
	if (log_len > 1) {
		GLchar *log_msg = new GLchar[log_len];
		glGetShaderInfoLog(id, log_len, nullptr, log_msg);
        if (status == GL_TRUE)
            note("Compiled successfully\n%s", log_msg);
        else
            error("The compilation has failed\n%s", log_msg);
        delete [] log_msg;
    }
    if (status != GL_TRUE)
        return EXIT_FAILURE;
    _shaders.push_back(id);
    return EXIT_SUCCESS;
}

int Shader_program::link_shaders()
{
    note("%s", "Link shaders...");
	GLuint id = glCreateProgram();
    if (!id) {
        error("%s", "Impossible to create a new program");
        return EXIT_FAILURE;
    }

    for (std::vector<GLuint>::iterator it = _shaders.begin() ; 
            it != _shaders.end() ; ++it)
        glAttachShader(id, *it);

    bind_attrib("vertex_position", SHADER_ATTRIB_POSITION);
    bind_attrib("vertex_color", SHADER_ATTRIB_COLOR);
    bind_attrib("vertex_texture", SHADER_ATTRIB_TEXTURE);
    bind_attrib("vertex_normal", SHADER_ATTRIB_NORMAL);

	glLinkProgram(id);

    GLint status, log_len;
	glGetProgramiv(id, GL_LINK_STATUS, &status);
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_len);
	if (log_len > 1)
    {
		GLchar *log_msg = new GLchar[log_len];
		glGetShaderInfoLog(id, log_len, nullptr, log_msg);
        if (status == GL_TRUE)
            note("Linked successfully\n%s", log_msg);
        else
            error("The linkage has failed\n%s", log_msg);
        delete [] log_msg;
    }
    if (status == GL_FALSE)
        return EXIT_FAILURE;

    for (std::vector<GLuint>::iterator it = _shaders.begin() ; 
            it != _shaders.end() ; ++it) {
        glDetachShader(id, *it);
        glDeleteShader(*it);
    }
    _shaders.clear();

    glDeleteProgram(_program_id);
    _program_id = id;
	
    bind_uniform_block("block_camera", SHADER_UNIFORM_BLOCK_CAMERA);
    bind_uniform_block("block_mesh", SHADER_UNIFORM_BLOCK_MESH);
    bind_uniform_block("block_precomputed", SHADER_UNIFORM_BLOCK_PRECOMPUTED);
    bind_uniform_block("block_light", SHADER_UNIFORM_BLOCK_LIGHT);
    bind_uniform_block("block_material", SHADER_UNIFORM_BLOCK_MATERIAL);

    print_summary();
    return EXIT_SUCCESS;
}

void Shader_program::use() const
{
    glUseProgram(_program_id);
}

void Shader_program::print_summary() const
{
    note("Summary of the shader program #%u", _program_id);

    GLint answer;
    glGetProgramiv(_program_id, GL_PROGRAM_BINARY_LENGTH, &answer);
    note("Size : %i bytes", answer);
    glGetProgramiv(_program_id, GL_DELETE_STATUS, &answer);
    note("Flagged for deletion : %s", (answer == GL_TRUE) ? "yes" : "no");
    glGetProgramiv(_program_id, GL_LINK_STATUS, &answer);
    note("Link status : %s", (answer == GL_TRUE) ? "success" : "failed");
    glGetProgramiv(_program_id, GL_VALIDATE_STATUS, &answer);
    note("Validate status : %s", (answer == GL_TRUE) ? "success" : "failed");
    glGetProgramiv(_program_id, GL_ACTIVE_ATOMIC_COUNTER_BUFFERS, &answer);
    note("%i buffer used", answer);
    glGetProgramiv(_program_id, GL_ATTACHED_SHADERS, &answer);
    note("%i shaders attached :", answer);
    {
        GLuint *shaders = new GLuint[answer];
        glGetAttachedShaders(_program_id, answer, nullptr, shaders);
        for (uint32_t index = 0 ; index < (uint32_t)answer ; ++index)
            note("  name:%u", shaders[index]);
        delete [] shaders;
    }
    glGetProgramiv(_program_id, GL_ACTIVE_ATTRIBUTES, &answer);
    note("%i attributes :", answer);
    for (uint32_t index = 0 ; index < (uint32_t)answer ; ++index) {
        const uint32_t bufSize = 32;
        int32_t size;
        GLenum type;
        char name[bufSize];
        glGetActiveAttrib(_program_id, index, bufSize, nullptr, &size, &type, name);
        note("  type:%u size:%i name:%s", type, size, name);
    }
    glGetProgramiv(_program_id, GL_ACTIVE_UNIFORMS, &answer);
    note("%i uniforms :", answer);
    for (uint32_t index = 0 ; index < (uint32_t)answer ; ++index) {
        const uint32_t bufSize = 32;
        int32_t size;
        GLenum type;
        char name[bufSize];
        glGetActiveUniform(_program_id, index, bufSize, nullptr, &size, &type, name);
        note("  type:%u size:%i name:%s", type, size, name);
    }
    glGetProgramiv(_program_id, GL_GEOMETRY_VERTICES_OUT, &answer);
    note("Geometry shader generates %u vert%s", (uint32_t)answer, (answer <= 1) ? "ex" : "ices");
    glGetProgramiv(_program_id, GL_GEOMETRY_INPUT_TYPE, &answer);
    note("Geometry shader takes type %u as input", (uint32_t)answer);
    glGetProgramiv(_program_id, GL_GEOMETRY_OUTPUT_TYPE, &answer);
    note("Geometry shader create type %u as output", (uint32_t)answer);
    glGetProgramiv(_program_id, GL_INFO_LOG_LENGTH, &answer);
	if (answer > 1)
    {
		GLchar *log_msg = new GLchar[answer];
		glGetShaderInfoLog(_program_id, answer, nullptr, log_msg);
        note("Info log : \n%s", log_msg);
        delete [] log_msg;
    }
}

inline void Shader_program::bind_attrib(const char *name, enum Attribute attrib)
{
    glBindAttribLocation(_program_id, attrib, name);
}

inline void Shader_program::bind_uniform_block(const char *name, enum Uniform_block block)
{
    GLuint index = glGetUniformBlockIndex(_program_id, name);
    if (index == GL_INVALID_INDEX)
        warning("Impossible to get the index of '%s'", name);
    else
        glUniformBlockBinding(_program_id, index, block);
}

