#ifndef _RENDER_H
#define _RENDER_H

#include <SDL2/SDL.h>
#include "map.h"
#include "player.h"
#include "texture.h"

class renderer
{
public:
    renderer(texture_manager* manager);
    void render(SDL_Surface* surface, SDL_Renderer* render, const game_map& map, const player& player);
    void draw_minimap(SDL_Renderer* renderer, const game_map& map);
    void draw_3d_fov(SDL_Surface* surface, const player& player);
    void draw_point_on_minimap(SDL_Renderer* renderer, const game_map& map, float x, float y);

private:
    texture_manager* manager;
};

#endif
