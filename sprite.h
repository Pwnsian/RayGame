#ifndef SPRITE_H
#define SPRITE_H
#include <cstdlib>
#include <SDL2/SDL.h>
#include "player.h"

class Sprite 
{
public:
    void render(SDL_Renderer* renderer, const player& p) const;

public:
    float x, y;
    size_t tex_id;
};

#endif
