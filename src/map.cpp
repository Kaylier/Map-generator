#include "map.hpp"

static uint8_t NB_FACE[256] = 
{
#include "../aux/nb_faces"
};
static uint8_t VERTEX_INDICES[256][7][3] = 
{
#include "../aux/index_faces"
};
static glm::vec3 VERTEX_COORD[13] = 
{
    glm::vec3(0.0f, 0.0f, 0.5f), /* V000_V001 */ 
    glm::vec3(0.0f, 0.5f, 0.0f), /* V000_V010 */ 
    glm::vec3(0.5f, 0.0f, 0.0f), /* V000_V100 */ 
    glm::vec3(0.0f, 0.5f, 1.0f), /* V001_V011 */ 
    glm::vec3(0.5f, 0.0f, 1.0f), /* V001_V101 */ 
    glm::vec3(0.0f, 1.0f, 0.5f), /* V010_V011 */ 
    glm::vec3(0.5f, 1.0f, 0.0f), /* V010_V110 */ 
    glm::vec3(0.5f, 1.0f, 1.0f), /* V011_V111 */ 
    glm::vec3(1.0f, 0.0f, 0.5f), /* V100_V101 */ 
    glm::vec3(1.0f, 0.5f, 0.0f), /* V100_V110 */ 
    glm::vec3(1.0f, 0.5f, 1.0f), /* V101_V111 */ 
    glm::vec3(1.0f, 1.0f, 0.5f), /* V110_V111 */ 
    glm::vec3(0.5f, 0.5f, 0.5f)  /* Center */
};

Map::Map() : 
    _seed(""), 
    _new_chunks(), 
    _new_chunks_mutex(), 
    _current_chunk(0,0,0)
{
    for (int32_t i = 0 ; i <= VIEW_DIST ; ++i)
        _quality_referer[i] = 128;
    for (int32_t i = 0 ; i < VIEW_SIZE ; ++i)
        for (int32_t j = 0 ; j < VIEW_SIZE ; ++j)
            for (int32_t k = 0 ; k < VIEW_SIZE ; ++k)
                _chunks[i][j][k] = nullptr;
}

Map::Map(const std::string &seed) : 
    _seed(seed), 
    _new_chunks(), 
    _new_chunks_mutex(), 
    _current_chunk(0,0,0)
{
    for (int32_t i = 0 ; i <= VIEW_DIST ; ++i)
        _quality_referer[i] = 128;
    for (int32_t i = 0 ; i < VIEW_SIZE ; ++i)
        for (int32_t j = 0 ; j < VIEW_SIZE ; ++j)
            for (int32_t k = 0 ; k < VIEW_SIZE ; ++k)
                _chunks[i][j][k] = nullptr;
}

Map::~Map()
{
    for (int32_t i = 0 ; i < VIEW_SIZE ; ++i)
        for (int32_t j = 0 ; j < VIEW_SIZE ; ++j)
            for (int32_t k = 0 ; k < VIEW_SIZE ; ++k)
                if (_chunks[i][j][k] != nullptr && _chunks[i][j][k] != (Map_chunk*)1) {
                    note("Deleting chunk [%2i][%2i][%2i]", i, j, k);
                    delete _chunks[i][j][k];
                }
}

void Map::set_seed(const std::string &seed)
{
    _seed = seed;
}

void Map::set_quality(uint8_t quality[VIEW_DIST])
{
    for (size_t i = 0 ; i < VIEW_DIST ; ++i)
        _quality_referer[i] = quality[i];
}

void Map::update(glm::vec3 center)
{
    int32_t i0 = (int32_t)center.x / MAP_CHUNK_SIZE - VIEW_DIST;
    if (center.x < 0) --i0;
    int32_t j0 = (int32_t)center.y / MAP_CHUNK_SIZE - VIEW_DIST;
    if (center.y < 0) --j0;
    int32_t k0 = (int32_t)center.z / MAP_CHUNK_SIZE - VIEW_DIST;
    if (center.z < 0) --k0;

    /* Start new computations (in thread ideally) */
    for (int32_t di = 0 ; di < VIEW_SIZE ; ++di)
        for (int32_t dj = 0 ; dj < VIEW_SIZE ; ++dj)
            for (int32_t dk = 0 ; dk < VIEW_SIZE ; ++dk) {

                int32_t i = (i0 + di) % (VIEW_SIZE);
                if (i < 0) i += VIEW_SIZE;
                int32_t j = (j0 + dj) % (VIEW_SIZE);
                if (j < 0) j += VIEW_SIZE;
                int32_t k = (k0 + dk) % (VIEW_SIZE);
                if (k < 0) k += VIEW_SIZE;

                int32_t x = (i0 + di) * MAP_CHUNK_SIZE;
                int32_t y = (j0 + dj) * MAP_CHUNK_SIZE;
                int32_t z = (k0 + dk) * MAP_CHUNK_SIZE;
                int32_t dist_max = abs(di-VIEW_DIST);
                if (abs(dj-VIEW_DIST) > dist_max)
                    dist_max = abs(dj-VIEW_DIST);
                if (abs(dk-VIEW_DIST) > dist_max)
                    dist_max = abs(dk-VIEW_DIST);
                uint8_t quality = _quality_referer[dist_max];

                if (_chunks[i][j][k] == nullptr) {
                    _chunks[i][j][k] = (Map_chunk*)1;
                    std::thread (&load, this, glm::ivec3(x, y, z), quality).detach();
                } else if (_chunks[i][j][k] != (Map_chunk*)1) {
                    if (_chunks[i][j][k]->position() != glm::ivec3(x, y, z) 
                     || _chunks[i][j][k]->quality() != quality) {
                        std::thread (&load, this, glm::ivec3(x, y, z), quality).detach();
                    }
                }
            }
    
    /* Import calculated chunks */
    _new_chunks_mutex.lock();
    while (!_new_chunks.empty()) {
        int32_t i = (_new_chunks.front()->position.x / MAP_CHUNK_SIZE) % (VIEW_SIZE);
        if (i < 0) i += VIEW_SIZE;
        int32_t j = (_new_chunks.front()->position.y / MAP_CHUNK_SIZE) % (VIEW_SIZE);
        if (j < 0) j += VIEW_SIZE;
        int32_t k = (_new_chunks.front()->position.z / MAP_CHUNK_SIZE) % (VIEW_SIZE);
        if (k < 0) k += VIEW_SIZE;
        if (_chunks[i][j][k] != nullptr && _chunks[i][j][k] != (Map_chunk*)1) {
            note("Replacing chunk [%2i][%2i][%2i] : (%i, %i, %i)", i, j, k, _new_chunks.front()->position.x, _new_chunks.front()->position.y, _new_chunks.front()->position.z);
            delete _chunks[i][j][k];
        } else {
            note("Adding chunk [%2i][%2i][%2i] : (%i, %i, %i)", i, j, k, _new_chunks.front()->position.x, _new_chunks.front()->position.y, _new_chunks.front()->position.z);
        }
        _chunks[i][j][k] = new Map_chunk(*_new_chunks.front());
        for (int i = 0 ; i < Shader_program::NB_SHADER_ATTRIB ; ++i)
            delete [] _new_chunks.front()->data[i];
        delete [] _new_chunks.front()->elements;
        delete _new_chunks.front();
        _new_chunks.pop();
    }
    _new_chunks_mutex.unlock();
}

void Map::update_uniforms(glm::mat4 &model_matrix) const
{
    if (_chunks[_current_chunk.x][_current_chunk.y][_current_chunk.z] != nullptr
     && _chunks[_current_chunk.x][_current_chunk.y][_current_chunk.z] != (Map_chunk*)1)
        _chunks[_current_chunk.x][_current_chunk.y][_current_chunk.z]->update_uniforms(model_matrix);
}

uint32_t Map::draw() const
{
    if (_chunks[_current_chunk.x][_current_chunk.y][_current_chunk.z] != nullptr
     && _chunks[_current_chunk.x][_current_chunk.y][_current_chunk.z] != (Map_chunk*)1)
        _chunks[_current_chunk.x][_current_chunk.y][_current_chunk.z]->draw();

    if (_current_chunk.x == 0) {
        _current_chunk.x = VIEW_SIZE-1;
        if (_current_chunk.y == 0) {
            _current_chunk.y = VIEW_SIZE-1;
            if (_current_chunk.z == 0) {
                _current_chunk.z = VIEW_SIZE-1;
                return 0;
            } else {
                --_current_chunk.z;
            }
        } else {
            --_current_chunk.y;
        }
    } else {
        --_current_chunk.x;
    }
    return 1;
}

inline int32_t Map::randi(glm::ivec2 pos, int32_t w) const
{
    int32_t ret = w;
    ret ^= pos.x;
    ret ^= pos.y;
    ret ^= pos.x << 2;
    ret ^= pos.y << 3;
    ret ^= pos.x >> 7;
    ret ^= pos.y >> 11;
    ret ^= (ret >> 17);
    ret ^= (ret << 19);
    //return ret % 89786857;
    ret ^= (ret / 23);
    return ret;
}

inline int32_t Map::randi(glm::ivec3 pos, int32_t w) const
{
    int32_t ret = w;
    ret ^= pos.x;
    ret ^= pos.y;
    ret ^= pos.z;
    ret ^= pos.x << 2;
    ret ^= pos.y << 3;
    ret ^= pos.z << 5;
    ret ^= pos.x >> 7;
    ret ^= pos.y >> 11;
    ret ^= pos.z >> 13;
    ret ^= (ret >> 17);
    return ret;
}

float Map::fast_perlin_noise(glm::ivec2 pos, int32_t size, float amplitude) const
{
    int32_t x0 = (pos.x/size) * size;
    if (pos.x < 0) x0 -= size;
    int32_t y0 = (pos.y/size) * size;
    if (pos.y < 0) y0 -= size;
    int32_t x1 = x0 + size;
    int32_t y1 = y0 + size;

    uint8_t vec[4]; {
    vec[0] = (uint32_t)randi(glm::ivec2(x0, y0), 0) % 4;
    vec[1] = (uint32_t)randi(glm::ivec2(x0, y1), 0) % 4;
    vec[2] = (uint32_t)randi(glm::ivec2(x1, y0), 0) % 4;
    vec[3] = (uint32_t)randi(glm::ivec2(x1, y1), 0) % 4;
    }
    //warning("vec: %i %i %i %i", vec[0], vec[1], vec[2], vec[3]);

    int dx0 = pos.x - x0;
    int dy0 = pos.y - y0;
    int dx1 = pos.x - x1;
    int dy1 = pos.y - y1;

    //warning("%i %i %i %i", dx0, dy0, dx1, dy1);
    int n0, n1, iy0, iy1;

    switch (vec[0]) {
        case 0 : n0 = + dx0 + dy0; break;
        case 1 : n0 = + dx0 - dy0; break;
        case 2 : n0 = - dx0 + dy0; break;
        case 3 : n0 = - dx0 - dy0; break;
    }
    switch (vec[1]) {
        case 0 : n1 = + dx0 + dy1; break;
        case 1 : n1 = + dx0 - dy1; break;
        case 2 : n1 = - dx0 + dy1; break;
        case 3 : n1 = - dx0 - dy1; break;
    }
    //warning("n0, n1 = %i, %i", n0, n1);
    iy0 = dy0*n1 - dy1*n0;
    switch (vec[2]) {
        case 0 : n0 = + dx1 + dy0; break;
        case 1 : n0 = + dx1 - dy0; break;
        case 2 : n0 = - dx1 + dy0; break;
        case 3 : n0 = - dx1 - dy0; break;
    }
    switch (vec[3]) {
        case 0 : n1 = + dx1 + dy1; break;
        case 1 : n1 = + dx1 - dy1; break;
        case 2 : n1 = - dx1 + dy1; break;
        case 3 : n1 = - dx1 - dy1; break;
    }
    //warning("n0, n1 = %i, %i", n0, n1);
    iy1 = dy0*n1 - dy1*n0;

    //warning("iy0, iy1 = %i, %i", iy0, iy1);
    //warning("%i", (dx0*iy1 - dx1*iy0));
    
    return (float)(dx0*iy1 - dx1*iy0) * amplitude / (size*size*size);
}

float Map::perlin_noise(glm::ivec2 pos, int32_t size, float amplitude) const
{
    /* TODO: perlin noise classic 2D */
    not_implemented();
    (void)pos; (void)size; (void)amplitude;
}

float Map::fast_perlin_noise(glm::ivec3 pos, int32_t size, float amplitude) const
{
    /* size max : 128, otherwise overflow risks */
    /* Random within a sample of eight vectors, whose coordinates are
     * (+/- 1, +/- 1, +/- 1). */
    int32_t x0 = (pos.x/size) * size;
    if (pos.x < 0) x0 -= size;
    int32_t y0 = (pos.y/size) * size;
    if (pos.y < 0) y0 -= size;
    int32_t z0 = (pos.z/size) * size;
    if (pos.z < 0) z0 -= size;
    int32_t x1 = x0 + size;
    int32_t y1 = y0 + size;
    int32_t z1 = z0 + size;

    uint8_t vec[8]; {
    vec[0] = (uint32_t)randi(glm::ivec3(x0, y0, z0), 0) % 12;
    vec[1] = (uint32_t)randi(glm::ivec3(x0, y0, z1), 0) % 12;
    vec[2] = (uint32_t)randi(glm::ivec3(x0, y1, z0), 0) % 12;
    vec[3] = (uint32_t)randi(glm::ivec3(x0, y1, z1), 0) % 12;
    vec[4] = (uint32_t)randi(glm::ivec3(x1, y0, z0), 0) % 12;
    vec[5] = (uint32_t)randi(glm::ivec3(x1, y0, z1), 0) % 12;
    vec[6] = (uint32_t)randi(glm::ivec3(x1, y1, z0), 0) % 12;
    vec[7] = (uint32_t)randi(glm::ivec3(x1, y1, z1), 0) % 12;
    }

    int dx0 = pos.x - x0;
    int dy0 = pos.y - y0;
    int dz0 = pos.z - z0;
    int dx1 = pos.x - x1;
    int dy1 = pos.y - y1;
    int dz1 = pos.z - z1;

    int n0, n1, iz0, iz1, izy0, izy1;

    switch (vec[0]) {
        case 0 : n0 = + dx0 + dy0 +   0; break;
        case 1 : n0 = + dx0 - dy0 +   0; break;
        case 2 : n0 = + dx0 +   0 + dz0; break;
        case 3 : n0 = + dx0 +   0 - dz0; break;
        case 4 : n0 = - dx0 - dy0 +   0; break;
        case 5 : n0 = - dx0 + dy0 +   0; break;
        case 6 : n0 = - dx0 +   0 - dz0; break;
        case 7 : n0 = - dx0 +   0 + dz0; break;
        case 8 : n0 = +   0 + dy0 + dz0; break;
        case 9 : n0 = +   0 + dy0 - dz0; break;
        case 10: n0 = +   0 - dy0 - dz0; break;
        case 11: n0 = +   0 - dy0 + dz0; break;
    }
    switch (vec[1]) {
        case 0 : n1 = + dx0 + dy0 +   0; break;
        case 1 : n1 = + dx0 - dy0 +   0; break;
        case 2 : n1 = + dx0 +   0 + dz1; break;
        case 3 : n1 = + dx0 +   0 - dz1; break;
        case 4 : n1 = - dx0 - dy0 +   0; break;
        case 5 : n1 = - dx0 + dy0 +   0; break;
        case 6 : n1 = - dx0 +   0 - dz1; break;
        case 7 : n1 = - dx0 +   0 + dz1; break;
        case 8 : n1 = +   0 + dy0 + dz1; break;
        case 9 : n1 = +   0 + dy0 - dz1; break;
        case 10: n1 = +   0 - dy0 - dz1; break;
        case 11: n1 = +   0 - dy0 + dz1; break;
    }
    iz0 = dz0*n1 - dz1*n0;
    switch (vec[2]) {
        case 0 : n0 = + dx0 + dy1 +   0; break;
        case 1 : n0 = + dx0 - dy1 +   0; break;
        case 2 : n0 = + dx0 +   0 + dz0; break;
        case 3 : n0 = + dx0 +   0 - dz0; break;
        case 4 : n0 = - dx0 - dy1 +   0; break;
        case 5 : n0 = - dx0 + dy1 +   0; break;
        case 6 : n0 = - dx0 +   0 - dz0; break;
        case 7 : n0 = - dx0 +   0 + dz0; break;
        case 8 : n0 = +   0 + dy1 + dz0; break;
        case 9 : n0 = +   0 + dy1 - dz0; break;
        case 10: n0 = +   0 - dy1 - dz0; break;
        case 11: n0 = +   0 - dy1 + dz0; break;
    }
    switch (vec[3]) {
        case 0 : n1 = + dx0 + dy1 +   0; break;
        case 1 : n1 = + dx0 - dy1 +   0; break;
        case 2 : n1 = + dx0 +   0 + dz1; break;
        case 3 : n1 = + dx0 +   0 - dz1; break;
        case 4 : n1 = - dx0 - dy1 +   0; break;
        case 5 : n1 = - dx0 + dy1 +   0; break;
        case 6 : n1 = - dx0 +   0 - dz1; break;
        case 7 : n1 = - dx0 +   0 + dz1; break;
        case 8 : n1 = +   0 + dy1 + dz1; break;
        case 9 : n1 = +   0 + dy1 - dz1; break;
        case 10: n1 = +   0 - dy1 - dz1; break;
        case 11: n1 = +   0 - dy1 + dz1; break;
    }
    iz1 = dz0*n1 - dz1*n0;
    izy0 = dy0*iz1 - dy1*iz0;
    switch (vec[4]) {
        case 0 : n0 = + dx1 + dy0 +   0; break;
        case 1 : n0 = + dx1 - dy0 +   0; break;
        case 2 : n0 = + dx1 +   0 + dz0; break;
        case 3 : n0 = + dx1 +   0 - dz0; break;
        case 4 : n0 = - dx1 - dy0 +   0; break;
        case 5 : n0 = - dx1 + dy0 +   0; break;
        case 6 : n0 = - dx1 +   0 - dz0; break;
        case 7 : n0 = - dx1 +   0 + dz0; break;
        case 8 : n0 = +   0 + dy0 + dz0; break;
        case 9 : n0 = +   0 + dy0 - dz0; break;
        case 10: n0 = +   0 - dy0 - dz0; break;
        case 11: n0 = +   0 - dy0 + dz0; break;
    }
    switch (vec[5]) {
        case 0 : n1 = + dx1 + dy0 +   0; break;
        case 1 : n1 = + dx1 - dy0 +   0; break;
        case 2 : n1 = + dx1 +   0 + dz1; break;
        case 3 : n1 = + dx1 +   0 - dz1; break;
        case 4 : n1 = - dx1 - dy0 +   0; break;
        case 5 : n1 = - dx1 + dy0 +   0; break;
        case 6 : n1 = - dx1 +   0 - dz1; break;
        case 7 : n1 = - dx1 +   0 + dz1; break;
        case 8 : n1 = +   0 + dy0 + dz1; break;
        case 9 : n1 = +   0 + dy0 - dz1; break;
        case 10: n1 = +   0 - dy0 - dz1; break;
        case 11: n1 = +   0 - dy0 + dz1; break;
    }
    iz0 = dz0*n1 - dz1*n0;
    switch (vec[6]) {
        case 0 : n0 = + dx1 + dy1 +   0; break;
        case 1 : n0 = + dx1 - dy1 +   0; break;
        case 2 : n0 = + dx1 +   0 + dz0; break;
        case 3 : n0 = + dx1 +   0 - dz0; break;
        case 4 : n0 = - dx1 - dy1 +   0; break;
        case 5 : n0 = - dx1 + dy1 +   0; break;
        case 6 : n0 = - dx1 +   0 - dz0; break;
        case 7 : n0 = - dx1 +   0 + dz0; break;
        case 8 : n0 = +   0 + dy1 + dz0; break;
        case 9 : n0 = +   0 + dy1 - dz0; break;
        case 10: n0 = +   0 - dy1 - dz0; break;
        case 11: n0 = +   0 - dy1 + dz0; break;
    }
    switch (vec[7]) {
        case 0 : n1 = + dx1 + dy1 +   0; break;
        case 1 : n1 = + dx1 - dy1 +   0; break;
        case 2 : n1 = + dx1 +   0 + dz1; break;
        case 3 : n1 = + dx1 +   0 - dz1; break;
        case 4 : n1 = - dx1 - dy1 +   0; break;
        case 5 : n1 = - dx1 + dy1 +   0; break;
        case 6 : n1 = - dx1 +   0 - dz1; break;
        case 7 : n1 = - dx1 +   0 + dz1; break;
        case 8 : n1 = +   0 + dy1 + dz1; break;
        case 9 : n1 = +   0 + dy1 - dz1; break;
        case 10: n1 = +   0 - dy1 - dz1; break;
        case 11: n1 = +   0 - dy1 + dz1; break;
    }
    iz1 = dz0*n1 - dz1*n0;
    izy1 = dy0*iz1 - dy1*iz0;

    return (float)(dx0*izy1 - dx1*izy0) * amplitude / (size*size*size*size);
}

float Map::perlin_noise(glm::ivec3 pos, int32_t size, float amplitude) const
{
    /* TODO: perlin noise classic 3D */
    not_implemented();
    (void)pos; (void)size; (void)amplitude;
}

float Map::value(glm::ivec3 pos, uint8_t quality) const
{
    /*
     * 0 : test meshes
     * 1 : basic landscape
     * 2 : rough terrain
     * 3 : city
     * 4 : canyons
     * 5 : desert
     * 6 : asteroids
     * 7 : realistic experiment
     */
    (void)quality;
    int selection = 4;

    float ret, tmp, high, floor0, floor1, floor2, ceil;
    switch (selection) {
        case 0:
            tmp = (pos.z > 3)? 1.f: -1.f;
            if (pos.x < 0 || pos.x > 63 || pos.y < 0 || pos.y > 63 || pos.z < 0 || pos.z > 7)
                ret = tmp;
            else if (pos.x & 0x02 || pos.y & 0x02 || pos.z & 0x02)
                ret = tmp;
            else if (pos.x%2 == 0 && pos.y%2 == 0 && pos.z%2 == 0)
                ret = (pos.x & 0x20)? -tmp : tmp;
            else if (pos.x%2 == 0 && pos.y%2 == 0 && pos.z%2 == 1)
                ret = (pos.y & 0x20)? -tmp : tmp;
            else if (pos.x%2 == 0 && pos.y%2 == 1 && pos.z%2 == 0)
                ret = (pos.x & 0x10)? -tmp : tmp;
            else if (pos.x%2 == 0 && pos.y%2 == 1 && pos.z%2 == 1)
                ret = (pos.y & 0x10)? -tmp : tmp;
            else if (pos.x%2 == 1 && pos.y%2 == 0 && pos.z%2 == 0)
                ret = (pos.x & 0x08)? -tmp : tmp;
            else if (pos.x%2 == 1 && pos.y%2 == 0 && pos.z%2 == 1)
                ret = (pos.y & 0x08)? -tmp : tmp;
            else if (pos.x%2 == 1 && pos.y%2 == 1 && pos.z%2 == 0)
                ret = (pos.x & 0x04)? -tmp : tmp;
            else
                ret = (pos.y & 0x04)? -tmp : tmp;

            return ret;
        case 1:
            ret = pos.z;
            ret += fast_perlin_noise(glm::ivec2(pos.x, pos.y), 64, 10.f);
            ret += fast_perlin_noise(glm::ivec2(pos.x, pos.y), 50, 8.f);
            return ret;
        case 2:
            ret = pos.z;
            ret += fast_perlin_noise(glm::ivec2(pos.x, pos.y), 32, 20.f);
            ret += fast_perlin_noise(pos, 32, 30.f);
            ret += fast_perlin_noise(pos, 22, 20.f);
            ret += fast_perlin_noise(pos, 8, 10.f);
            ret += fast_perlin_noise(pos, 5, 6.f);
            return ret;
        case 3:
            high = 10.f;
            high -= fast_perlin_noise(glm::ivec2(pos.x, pos.y), 30, 10.f);
            high *= 10000.f/(10000.f + pos.x*pos.x + pos.y*pos.y);
            ret = pos.z;
            ret -= fast_perlin_noise(glm::ivec2(pos.x, pos.y), 40, 15.f);
            ret -= high * (1.f + fast_perlin_noise(glm::ivec2(pos.x, pos.y), 2, 1.f));
            return ret;
        case 4:
            floor0 = 0.f;
            floor1 = 10.f;
            floor2 = 20.f;
            ceil = 30.f;
            if (pos.z < floor0)
                return -10.f;
            else if (pos.z < floor1)
                ret = -10.f;
            else if (pos.z < floor1)
                ret = 0.f;
            else if (pos.z < floor2)
                ret = 10.f;
            else if (pos.z < ceil)
                ret = 20.f;
            else
                return 30.f;
            high = fast_perlin_noise(glm::ivec2(pos.x, pos.y), 40, 1.f);
            high = fast_perlin_noise(glm::ivec2(pos.x, pos.y), 35, 0.8f);
            high = std::abs(high);
            ret -= high * 40.f + 5.f;
            ret -= fast_perlin_noise(pos, 32, 15.f);
            ret -= fast_perlin_noise(pos, 22, 10.f);
            ret -= fast_perlin_noise(pos, 8, 8.f);
            return ret;
        case 5:
            floor0 = 5.f;
            floor0 -= fast_perlin_noise(glm::ivec2(pos.x*2, pos.y), 40, 3.f);
            floor0 -= fast_perlin_noise(glm::ivec2(pos.x*3/2, pos.y), 32, 2.f);
            if (pos.z < floor0)
                return -10.f;
            else
                ret = pos.z;
            ret -= fast_perlin_noise(pos, 12, 12.f);
            ret -= fast_perlin_noise(pos, 8, 10.f);
            return ret;
        case 6:
            tmp = sqrt(pos.x*pos.x + pos.y*pos.y) - 128.f;
            tmp = tmp*tmp + pos.z*pos.z;
            ret = 1.5f - 1000.f / (1000.f + tmp);
            ret += fast_perlin_noise(pos, 3, 1.3f);
            return ret;
        case 7:
            ret = pos.z;
            high = fast_perlin_noise(glm::ivec2(pos.x, pos.y), 128, 1.f);
            high += fast_perlin_noise(glm::ivec2(pos.x, pos.y), 115, 1.f);
            ret += high*fast_perlin_noise(pos, 8, 8.f);
            high -= 1.f;
            if (high < -1.f)
                high += 2.f;
            high = high * high * high;
            ret -= high*(0.8f+fast_perlin_noise(pos, 16, 32.f));
            return ret;
        default:
            return pos.z;
    }
}

void Map::load(Map *map, glm::ivec3 pos, uint8_t quality)
{
    note("Creating chunk (%i, %i, %i) with quality [%u]", pos.x, pos.y, pos.z, quality);

    Map_chunk::Args *new_chunk = new Map_chunk::Args;
    new_chunk->position = pos;
    new_chunk->quality = quality;

    /* Calcul points values */
    bool values[MAP_CHUNK_SIZE+1][MAP_CHUNK_SIZE+1][MAP_CHUNK_SIZE+1]; {
        size_t i, j, k;
        glm::ivec3 p;
        i = 0;
        p.x = pos.x;
        while (i <= MAP_CHUNK_SIZE) {
            j = 0;
            p.y = pos.y;
            while (j <= MAP_CHUNK_SIZE) {
                k = 0;
                p.z = pos.z;
                while (k <= MAP_CHUNK_SIZE) {
                    values[i][j][k] = (map->value(p, quality) > 0.f) ? true : false;
                    ++k;
                    ++p.z;
                }
                ++j;
                ++p.y;
            }
            ++i;
            ++p.x;
        }
    }

    /* List used vertices and calcul their normals */
    /*std::unordered_map<glm::vec3, size_t, hash_vec3> index;
    for (size_t i = 0 ; i < MAP_CHUNK_SIZE ; ++i)
        for (size_t j = 0 ; j < MAP_CHUNK_SIZE ; ++j)
            for (size_t k = 0 ; k < MAP_CHUNK_SIZE ; ++k)
                if (values[i][j][k] != values[i+1][j][k]) {
                    vertices.emplace(vertices.end(), 
                            glm::vec3((float)pos.x + (float)i + 0.5f, 
                                (float)pos.y + (float)j, 
                                (float)pos.z + (float)k));
                    index.emplace(vertices.back(), vertices.size()-1);
                }
    for (size_t i = 0 ; i < MAP_CHUNK_SIZE ; ++i)
        for (size_t j = 0 ; j < MAP_CHUNK_SIZE ; ++j)
            for (size_t k = 0 ; k < MAP_CHUNK_SIZE ; ++k)
                if (values[i][j][k] != values[i+1][j][k]) {
                    vertices.emplace(vertices.end(), 
                            glm::vec3((float)pos.x + (float)i, 
                                (float)pos.y + (float)j + 0.5f, 
                                (float)pos.z + (float)k));
                    index.emplace(vertices.back(), vertices.size()-1);
                }
    for (size_t i = 0 ; i < MAP_CHUNK_SIZE ; ++i)
        for (size_t j = 0 ; j < MAP_CHUNK_SIZE ; ++j)
            for (size_t k = 0 ; k < MAP_CHUNK_SIZE ; ++k)
                if (values[i][j][k] != values[i+1][j][k]) {
                    vertices.emplace(vertices.end(), 
                            glm::vec3((float)pos.x + (float)i, 
                                (float)pos.y + (float)j, 
                                (float)pos.z + (float)k) + 0.5f);
                    index.emplace(vertices.back(), vertices.size()-1);
                }
    */
    /* TODO: optimize meshes created (profit of elements) */
    
    int32_t nb_face = 0;
    for (size_t i = 0 ; i < MAP_CHUNK_SIZE ; ++i)
        for (size_t j = 0 ; j < MAP_CHUNK_SIZE ; ++j)
            for (size_t k = 0 ; k < MAP_CHUNK_SIZE ; ++k) {
                uint8_t v = 0;
                if (values[i  ][j  ][k  ]) v |= 0x01;
                if (values[i  ][j  ][k+1]) v |= 0x02;
                if (values[i  ][j+1][k  ]) v |= 0x04;
                if (values[i  ][j+1][k+1]) v |= 0x08;
                if (values[i+1][j  ][k  ]) v |= 0x10;
                if (values[i+1][j  ][k+1]) v |= 0x20;
                if (values[i+1][j+1][k  ]) v |= 0x40;
                if (values[i+1][j+1][k+1]) v |= 0x80;
                nb_face += NB_FACE[v];
            }

    new_chunk->data[Shader_program::SHADER_ATTRIB_POSITION] = new float[nb_face*9];
    new_chunk->data[Shader_program::SHADER_ATTRIB_COLOR] = new float[nb_face*9];
    new_chunk->data[Shader_program::SHADER_ATTRIB_TEXTURE] = new float[nb_face*6];
    new_chunk->data[Shader_program::SHADER_ATTRIB_NORMAL] = new float[nb_face*9];
    new_chunk->elements = new uint32_t[nb_face*3];
    if (! new_chunk->data[Shader_program::SHADER_ATTRIB_POSITION]
     || ! new_chunk->data[Shader_program::SHADER_ATTRIB_COLOR]
     || ! new_chunk->data[Shader_program::SHADER_ATTRIB_TEXTURE]
     || ! new_chunk->data[Shader_program::SHADER_ATTRIB_NORMAL]
     || ! new_chunk->elements)
    {
        delete [] new_chunk->data[Shader_program::SHADER_ATTRIB_POSITION];
        delete [] new_chunk->data[Shader_program::SHADER_ATTRIB_COLOR];
        delete [] new_chunk->data[Shader_program::SHADER_ATTRIB_TEXTURE];
        delete [] new_chunk->data[Shader_program::SHADER_ATTRIB_NORMAL];
        delete [] new_chunk->elements;
        error("%s", "Impossible to allocate needed memory");
        return;
    }
    new_chunk->vertex_count = nb_face * 3;
    new_chunk->element_count = nb_face * 3;

    float *positions = new_chunk->data[Shader_program::SHADER_ATTRIB_POSITION];
    float *colors = new_chunk->data[Shader_program::SHADER_ATTRIB_COLOR];
    float *normals = new_chunk->data[Shader_program::SHADER_ATTRIB_NORMAL];
    uint32_t p = 0;
    for (int i = 0 ; i < MAP_CHUNK_SIZE ; ++i)
        for (int j = 0 ; j < MAP_CHUNK_SIZE ; ++j)
            for (int k = 0 ; k < MAP_CHUNK_SIZE ; ++k) {
                uint8_t v = 0;
                if (values[i  ][j  ][k  ]) v |= 0x01;
                if (values[i  ][j  ][k+1]) v |= 0x02;
                if (values[i  ][j+1][k  ]) v |= 0x04;
                if (values[i  ][j+1][k+1]) v |= 0x08;
                if (values[i+1][j  ][k  ]) v |= 0x10;
                if (values[i+1][j  ][k+1]) v |= 0x20;
                if (values[i+1][j+1][k  ]) v |= 0x40;
                if (values[i+1][j+1][k+1]) v |= 0x80;

                for (size_t n = 0 ; n < NB_FACE[v] ; ++n)
                {
                    glm::vec3 color(1.f - (float)quality/255.f, 0.8f, 0.5f);
                    glm::vec3 normal;

                    normal.x = (VERTEX_COORD[VERTEX_INDICES[v][n][1]].y - VERTEX_COORD[VERTEX_INDICES[v][n][0]].y)
                        * (VERTEX_COORD[VERTEX_INDICES[v][n][2]].z - VERTEX_COORD[VERTEX_INDICES[v][n][0]].z) 
                        - (VERTEX_COORD[VERTEX_INDICES[v][n][1]].z - VERTEX_COORD[VERTEX_INDICES[v][n][0]].z)
                        * (VERTEX_COORD[VERTEX_INDICES[v][n][2]].y - VERTEX_COORD[VERTEX_INDICES[v][n][0]].y);
                    normal.y = (VERTEX_COORD[VERTEX_INDICES[v][n][1]].z - VERTEX_COORD[VERTEX_INDICES[v][n][0]].z)
                        * (VERTEX_COORD[VERTEX_INDICES[v][n][2]].x - VERTEX_COORD[VERTEX_INDICES[v][n][0]].x) 
                        - (VERTEX_COORD[VERTEX_INDICES[v][n][1]].x - VERTEX_COORD[VERTEX_INDICES[v][n][0]].x)
                        * (VERTEX_COORD[VERTEX_INDICES[v][n][2]].z - VERTEX_COORD[VERTEX_INDICES[v][n][0]].z);
                    normal.z = (VERTEX_COORD[VERTEX_INDICES[v][n][1]].x - VERTEX_COORD[VERTEX_INDICES[v][n][0]].x)
                        * (VERTEX_COORD[VERTEX_INDICES[v][n][2]].y - VERTEX_COORD[VERTEX_INDICES[v][n][0]].y) 
                        - (VERTEX_COORD[VERTEX_INDICES[v][n][1]].y - VERTEX_COORD[VERTEX_INDICES[v][n][0]].y)
                        * (VERTEX_COORD[VERTEX_INDICES[v][n][2]].x - VERTEX_COORD[VERTEX_INDICES[v][n][0]].x);
                    float r = (float)sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
                    normal.x /= r;
                    normal.y /= r;
                    normal.z /= r;

                    new_chunk->elements[p/3] = p/3;
                    positions[p] = (float)i + VERTEX_COORD[VERTEX_INDICES[v][n][0]].x;
                    colors[p] = color.x;
                    normals[p] = normal.x;
                    ++p;
                    positions[p] = (float)j + VERTEX_COORD[VERTEX_INDICES[v][n][0]].y;
                    colors[p] = color.y;
                    normals[p] = normal.y;
                    ++p;
                    positions[p] = (float)k + VERTEX_COORD[VERTEX_INDICES[v][n][0]].z;
                    colors[p] = color.z;
                    normals[p] = normal.z;
                    ++p;
                    new_chunk->elements[p/3] = p/3;
                    positions[p] = (float)i + VERTEX_COORD[VERTEX_INDICES[v][n][1]].x;
                    colors[p] = color.x;
                    normals[p] = normal.x;
                    ++p;
                    positions[p] = (float)j + VERTEX_COORD[VERTEX_INDICES[v][n][1]].y;
                    colors[p] = color.y;
                    normals[p] = normal.y;
                    ++p;
                    positions[p] = (float)k + VERTEX_COORD[VERTEX_INDICES[v][n][1]].z;
                    colors[p] = color.z;
                    normals[p] = normal.z;
                    ++p;
                    new_chunk->elements[p/3] = p/3;
                    positions[p] = (float)i + VERTEX_COORD[VERTEX_INDICES[v][n][2]].x;
                    colors[p] = color.x;
                    normals[p] = normal.x;
                    ++p;
                    positions[p] = (float)j + VERTEX_COORD[VERTEX_INDICES[v][n][2]].y;
                    colors[p] = color.y;
                    normals[p] = normal.y;
                    ++p;
                    positions[p] = (float)k + VERTEX_COORD[VERTEX_INDICES[v][n][2]].z;
                    colors[p] = color.z;
                    normals[p] = normal.z;
                    ++p;
                }
            }
    map->_new_chunks_mutex.lock();
    map->_new_chunks.push(new_chunk);
    map->_new_chunks_mutex.unlock();
}

