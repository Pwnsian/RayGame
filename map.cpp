#include "map.h"
#include "constants.h"
   
game_map::game_map(texture_manager* manager)
    : manager(manager)
{
    // Map
    std::array<std::array<char,16>,16> map = // our game map
    {{
        {'0','0','0','0','2','2','2','2','2','2','2','2','0','0','0','0'},
        {'1',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','0'},
        {'1',' ',' ',' ',' ',' ',' ','1','1','1','1','1',' ',' ',' ','0'},
        {'1',' ',' ',' ',' ',' ','0',' ',' ',' ',' ',' ',' ',' ',' ','0'},
        {'0',' ',' ',' ',' ',' ','0',' ',' ','1','1','1','0','0','0','0'},
        {'0',' ',' ',' ',' ',' ','3',' ',' ',' ',' ',' ',' ',' ',' ','0'},
        {'0',' ',' ',' ','1','0','0','0','0',' ',' ',' ',' ',' ',' ','0'},
        {'0',' ',' ',' ','0',' ',' ',' ','1','1','1','0','0',' ',' ','0'},
        {'4',' ',' ',' ','4',' ',' ',' ','0',' ',' ',' ',' ',' ',' ','0'},
        {'4',' ',' ',' ','4',' ',' ',' ','1',' ',' ','0','0','0','0','0'},
        {'0',' ',' ',' ',' ',' ',' ',' ','1',' ',' ',' ',' ',' ',' ','0'},
        {'2',' ',' ',' ',' ',' ',' ',' ','1',' ',' ',' ',' ',' ',' ','0'},
        {'0',' ',' ',' ',' ',' ',' ',' ','0',' ',' ',' ',' ',' ',' ','0'},
        {'0',' ','0','0','0','0','0','0','0',' ',' ',' ',' ',' ',' ','0'},
        {'0',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','0'},
        {'0','0','0','2','2','2','2','2','2','2','2','0','0','0','0','0'}
    }};

    for(int row = 0; row < map.size(); ++row)
    for(int col = 0; col < map[0].size(); ++col)
    {
        map_data[row][col] = (texture_type)map[row][col];
    }
}

game_map::~game_map()
{

}

ray_cast_result game_map::trace_ray(float start_x, float start_y, float angle, ray_cast_visitor visitor) const
{
    ray_cast_result result;

    for (float c = 0; c < 20; c += .05)
    {
        float x = start_x + c * cos(angle);
        float y = start_y + c * sin(angle);

        visitor(x, y);

        texture_type map_x_y = map_data[(int)y][(int)x];
        if (map_x_y != texture_type::TEXTURE_NONE)
        {
            result.distance = c;
            result.x = x;
            result.y = y;
            result.tile_hit = map_x_y;
            break;
        }
    }

    return result;
}

void game_map::draw_minimap(SDL_Renderer* renderer) const
{
    const size_t map_h = map_data.size();
    const size_t map_w = map_data[0].size();
    int block_size_w = (map_render_size_w / map_w);
    int block_size_h = (map_render_size_h / map_h);

    for(int row = 0; row < map_h; ++row)
    for(int col = 0; col < map_w; ++col)
    {
        texture_type map_i_j = map_data[row][col];
        if(map_i_j == texture_type::TEXTURE_NONE)
            continue;

        SDL_Rect wall { to_pixels_x(col), to_pixels_y(row), block_size_w, block_size_h };
        SDL_Color wall_color = manager->get_pixel(texture_name::TEXTURES_WALL, 64 * ((char)map_i_j - '0'), 0);
        SDL_SetRenderDrawColor(renderer, wall_color.r, wall_color.g, wall_color.b, wall_color.a);
        SDL_RenderFillRect(renderer, &wall);
    }
}

int game_map::to_pixels_x(float map_position) const
{
    // Map width in absolute coordinates
    const size_t map_w = map_data[0].size();

    // How large a block is in pixels assuming the map is divided evenly
    int block_size_w = (map_render_size_w / map_w);

    return  map_position * block_size_w;
}

int game_map::to_pixels_y(float map_position) const
{
    // Map height in absolute coordinates
    const size_t map_y = map_data.size();

    // How large a block is in pixels assuming the map is divided evenly
    int block_size_h = (map_render_size_h / map_y);

    return  map_position * block_size_h;
}