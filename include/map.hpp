#ifndef MAP_HPP
#define MAP_HPP

//#include <cmath>
#include <thread>
#include <mutex>
#include <string>
#include <queue>
#include <cstdint> /* uint8_t, int32_t, uint32_t */
#include "glm/vec3.hpp" /* glm::ivec3, glm::vec3 */
#include "glm/mat4x4.hpp" /* glm::dmat4 */
#include "glm/gtc/noise.hpp" /* glm::perlin, glm::simplex */

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

//#include "lookup_vector.hpp"
#include "map_chunk.hpp"

#include "debug.hpp"

#define VIEW_DIST 3 /* Distance of the further chunk */
#define VIEW_SIZE 2*VIEW_DIST+1

class Map
{
    /* TODO: complete keywords like virtual, final, private, ... */
public:
    Map();
    Map(const Map&) = delete;
    Map(const std::string &seed);
    virtual ~Map();

    Map &operator=(const Map&) = delete;

    void set_seed(const std::string &seed);
    void set_quality(uint8_t quality[VIEW_DIST]);

    void update(glm::vec3 center);
    virtual void update_uniforms(glm::mat4 &model_matrix) const;
    virtual uint32_t draw() const;

protected:
    std::string _seed;
    uint8_t _quality_referer[VIEW_DIST+1]; /* 0:lowest - 255:highest quality */
    
    Map_chunk *_chunks[VIEW_SIZE][VIEW_SIZE][VIEW_SIZE];
    std::queue<Map_chunk::Args*> _new_chunks;
    std::mutex _new_chunks_mutex;

    mutable glm::ivec3 _current_chunk;

    inline int32_t randi(glm::ivec2 pos, int32_t w) const;
    inline int32_t randi(glm::ivec3 pos, int32_t w) const;
    float fast_perlin_noise(glm::ivec2 pos, int32_t size, float amplitude) const;
    float perlin_noise(glm::ivec2 pos, int32_t size, float amplitude) const;
    float fast_perlin_noise(glm::ivec3 pos, int32_t size, float amplitude) const;
    float perlin_noise(glm::ivec3 pos, int32_t size, float amplitude) const;
    float value(glm::ivec3 pos, uint8_t quality) const;

private:
    static void load(Map *map, glm::ivec3 pos, uint8_t quality);
};

#endif // MAP_HPP
