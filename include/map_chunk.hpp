#ifndef MAP_CHUNK_HPP
#define MAP_CHUNK_HPP

#include <cstdint> /* uint8_t, uint32_t */
#include "glm/vec3.hpp" /* glm::ivec3, glm::vec3 */
#include "glm/mat4x4.hpp" /* glm::mat4 */
#include "glm/gtc/matrix_transform.hpp" /* glm::translate */

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "drawable.hpp"
#include "shader_program.hpp"

#define MAP_CHUNK_SIZE 32 /* number of blocs of a chunk on all axis */

class Map_chunk : public Drawable
{
public:
    struct Args
    {
        glm::ivec3 position;
        uint8_t quality;
        float *data[Shader_program::NB_SHADER_ATTRIB];
        //float (*data)[11];
        int32_t vertex_count;
        uint32_t *elements;
        int32_t element_count;
    };

    Map_chunk() = delete;
    Map_chunk(const Map_chunk&) = delete;
    Map_chunk(const struct Args &arg);
    Map_chunk(glm::ivec3 position, uint8_t quality, 
              const float *const data[Shader_program::NB_SHADER_ATTRIB], int32_t vertex_count, 
              const uint32_t elements[], int32_t element_count);
    Map_chunk(glm::ivec3 position, uint8_t quality, 
              const float (*const data)[11], int32_t vertex_count, 
              const uint32_t elements[], int32_t element_count);
    virtual ~Map_chunk() = default;

    Map_chunk &operator=(const Map_chunk&) = delete;

    glm::ivec3 position() const;
    uint8_t quality() const;

    virtual void update_uniforms(glm::mat4 &model_matrix) const override;

    static void print_summary(const struct Args &arg);
     
protected:
    const glm::ivec3 _position;
    const uint8_t _quality;

private:

};

#endif // MAP_CHUNK_HPP
