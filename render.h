#ifndef _RENDER_H
#define _RENDER_H

#include <SDL2/SDL.h>
#include <vector>
#include "map.h"
#include "player.h"
#include "texture.h"
#include "sprite.h"

class renderer
{
public:
    renderer(texture_manager* manager);
    void render(SDL_Surface* surface, SDL_Renderer* sdl_renderer, const game_map& map, const player& player);
    void render(SDL_Surface* surface, SDL_Renderer* sdl_renderer, const game_map& map, const player& player,
                const std::vector<Sprite>& sprites);    

private:
    texture_manager* manager;
    void draw_point_on_minimap(SDL_Renderer* renderer, const game_map& map, float x, float y);    
};

#endif
