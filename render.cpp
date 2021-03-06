#include <iostream>
#include <algorithm>
#include "render.h"
#include "constants.h"

renderer::renderer(texture_manager* manager)
    : manager(manager)
{

}

void renderer::render(SDL_Surface* surface, SDL_Renderer* sdl_renderer, const game_map& map, const player& player)
{
    std::vector<Sprite> emptySprits;
    render(surface, sdl_renderer, map, player, emptySprits);
}

void renderer::render(
    SDL_Surface* surface, 
    SDL_Renderer* sdl_renderer, 
    const game_map& map, 
    const player& p,
    const std::vector<Sprite>& sprites)
{
    using namespace std::placeholders;
    auto draw_ray_on_map = std::bind(&renderer::draw_point_on_minimap, this, sdl_renderer, map, _1, _2);

    // Draw map
    map.draw_minimap(sdl_renderer);

    // Draw player
    SDL_Rect playerpos { map.to_pixels_x(p.x), map.to_pixels_y(p.y), 8, 8 };
    SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(sdl_renderer, &playerpos); 

    // Draw Rays on player FOV, and then 3D image on the right half of the surface.
    // Also record rays hit for sprite draw depth buffer
    float depth_buffer[512];
    float start_ang = p.a - p.fov / 2.0;
    float end_ang = p.a + p.fov / 2.0;
    int ray_number = 0;
    for(float current_ang = start_ang; current_ang < end_ang; current_ang += (p.fov / 512.0), ++ray_number)
    {
        ray_cast_result cast_result = map.trace_ray(p.x, p.y, current_ang, draw_ray_on_map);
        depth_buffer[ray_number] = cast_result.distance;

        float ray_dist = cast_result.distance;
        texture_type tile_hit = cast_result.tile_hit;
        float corrected_ray_dist = ray_dist * cos(current_ang - p.a);

        int column_x = map_render_size_w + ray_number;
        int column_w = 1;
        int column_h = (corrected_ray_dist > 0) ? full_image_size_h / corrected_ray_dist : corrected_ray_dist;
        int column_y = full_image_size_h/2 - column_h/2;

        // Determine vertical or horizontal wall by comparing the fractional part
        // if the ray hit. The corresponding direction that 'sweeps' is what to use in texturing.
        float hit_x = cast_result.x - floor(cast_result.x + 0.5);
        float hit_y = cast_result.y - floor(cast_result.y + 0.5);
        int textcord_x = texture_width * hit_x;
        if(abs(hit_y) > abs(hit_x))
            textcord_x = texture_width * hit_y;
        if (textcord_x < 0)
            textcord_x += texture_width; // do not forget textcord_x can be negative, fix that
        int texture_index = (char)cast_result.tile_hit - '0';

        SDL_Rect texture_rect { (texture_width * texture_index) + textcord_x, 0, 1, texture_width };
        SDL_Rect dest_rect { column_x, column_y, column_w, column_h};
        SDL_BlitScaled(manager->get_texture(texture_name::TEXTURES_WALL), &texture_rect, surface, &dest_rect);
    }

    // Draw Sprites - Sort by player distance to not draw over
    std::vector<const Sprite*> sortedSprites;
    std::for_each(sprites.begin(), sprites.end(), [&](auto& s){ sortedSprites.push_back(&s); });
    auto playDistanceSort = [&](const Sprite* s1, const Sprite* s2) {
        return std::hypot(s1->x - p.x, s1->y - p.y) > std::hypot(s2->x - p.x, s2->y - p.y);
    };
    std::sort(sortedSprites.begin(), sortedSprites.end(), playDistanceSort);

    for(const Sprite* s : sortedSprites)
    {
        // Draw minimap
        SDL_Rect spritepos { map.to_pixels_x(s->x), map.to_pixels_y(s->y), 8, 8 };
        SDL_SetRenderDrawColor(sdl_renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(sdl_renderer, &spritepos); 

        // Draw 3D
        s->render(sdl_renderer, surface, manager, p, depth_buffer);
    }    
}

void renderer::draw_point_on_minimap(SDL_Renderer* renderer, const game_map& map, float x, float y)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderer, map.to_pixels_x(x), map.to_pixels_y(y));  
}
