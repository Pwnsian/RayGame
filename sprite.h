#ifndef SPRITE_H
#define SPRITE_H
#include <cstdlib>
#include <SDL2/SDL.h>
#include "player.h"
#include "texture.h"

class Sprite 
{
public:
    void render(SDL_Renderer* renderer,
                SDL_Surface* surface,
                texture_manager* texture_manager,
                const player& p, 
                float depth_buffer[512]) const;
public:
    float x, y;
    size_t tex_id;
};

#endif
