#ifndef PLAYER_H
#define PLAYER_H
#include <cmath>

struct player
{
    player() : x(0), y(0), a(0), fov(M_PI/3)
    { }
    float x;
    float y;
    float a;
    float fov;
};

#endif