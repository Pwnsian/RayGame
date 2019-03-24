#include "sprite.h"
#include "constants.h"

void Sprite::render(SDL_Renderer* renderer, const player& p) const
{
    // Draw Minimap

    // Draw 3D FoV
    float sprite_dir = std::atan2(y - p.y, x - p.x);   

    // Positive: Sprite is to the right of the player.
    // Negative: To the left.
    // Use this to place the horizontal offset of the sprite
    float sprite_offset = sprite_dir - p.a;

    // Compute size of seen square based on distance
    float sprite_dist = std::hypot(p.x - x, p.y - y);
    int sprite_width = map_render_size_w / sprite_dist;
    int sprite_height = map_render_size_h / sprite_dist;

    // Offset from middle of fov (p.a) multiplied by how many fovs there are in the map render size
    float sz = sprite_offset * (map_render_size_w / p.fov);

    int sprite_x = map_render_size_w + (map_render_size_w / 2) + sz - (sprite_width / 2);
    int sprite_y = (full_image_size_h / 2) - sprite_height / 2;

    // Save clip rect, then clip the drawing to the 3D render view only. 
    // Should this be moved outside of this?
    SDL_Rect prev_clip_rect;
    SDL_Rect threed_view_rect = { map_render_size_w, 0, map_render_size_w, map_render_size_h };    
    SDL_RenderGetClipRect(renderer, &prev_clip_rect);
    SDL_RenderSetClipRect(renderer, &threed_view_rect);

    SDL_Rect sprite_rect = { sprite_x, sprite_y, sprite_width, sprite_height };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &sprite_rect);

    SDL_RenderSetClipRect(renderer, &prev_clip_rect);
}
