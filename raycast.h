#ifndef RAY_CAST_H
#define RAY_CAST_H

#include "texture.h"

struct ray_cast_result
{
    ray_cast_result() : distance(0), x(0), y(0), tile_hit(texture_type::TEXTURE_NONE)
    { }
    float distance;
    float x;
    float y;
    texture_type tile_hit;
};

#endif
