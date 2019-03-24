#include "sprite.h"
#include "constants.h"

void Sprite::render(
    SDL_Renderer* renderer,
    SDL_Surface* surface,
    texture_manager* texture_manager,
    const player& p, 
    float depth_buffer[512]) const
{
    // Draw Minimap
    // Currently done outside, move here? Thinking not.

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

    int sprite_x_from_center = (map_render_size_w / 2) + sz - (sprite_width / 2);
    int sprite_y = (full_image_size_h / 2) - sprite_height / 2;

    // Save clip rect, then clip the drawing to the 3D render view only. 
    // Should this be moved outside of this?
    SDL_Rect prev_clip_rect;
    SDL_Rect threed_view_rect = { map_render_size_w, 0, map_render_size_w, map_render_size_h };    
    SDL_GetClipRect(surface, &prev_clip_rect);
    SDL_SetClipRect(surface, &threed_view_rect);

    // Draw each column separately to clip the depth buffer
    for(int column = 0; column < sprite_width; ++column)
    {
        int sprite_column_x = sprite_x_from_center + column;
        int sprite_depth_column = sprite_column_x - map_render_size_w;

        // FIXME!
        //if(sprite_depth_column < 0 || sprite_depth_column >= map_render_size_w)
        //    continue;
        //
        //if(sprite_dist < depth_buffer[sprite_depth_column])
        //    continue;

        // Linear interpolate to map (0..sprite_width) into (0..texture_width) to draw columns
        // to best fit the expanded or reduced sprite width.
        int texture_column =  0 + (float(texture_width) / float(sprite_width)) * column;

        SDL_Surface* monsters = texture_manager->get_texture(texture_name::TEXTURES_MONSTERS);
        SDL_Rect src_rect = { ((int)tex_id * texture_width) + texture_column, 0, 1, texture_width };
        SDL_Rect dst_rect = { map_render_size_w + sprite_x_from_center + column, sprite_y, 1, sprite_height };    
        SDL_BlitScaled(monsters, &src_rect, surface, &dst_rect);
    }

    SDL_SetClipRect(surface, &prev_clip_rect);
}
