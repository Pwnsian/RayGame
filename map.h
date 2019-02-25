#ifndef GAME_MAP_H
#define GAME_MAP_H

#include <SDL2/SDL.h>
#include <functional>
#include "raycast.h"
#include "texture.h"

typedef std::function<void(float x, float y)> ray_cast_visitor;

class game_map
{
public:    
    game_map(texture_manager* manager);
    ~game_map();
    ray_cast_result trace_ray(float start_x, float start_y, float angle, ray_cast_visitor visitor) const;
    void draw_minimap(SDL_Renderer* surface) const;
    int to_pixels_x(float map_position) const;
    int to_pixels_y(float map_position) const;

private:
    typedef std::array<std::array<texture_type,16>,16> map_data_type;
    texture_manager* manager;
    map_data_type map_data;
};

#endif
