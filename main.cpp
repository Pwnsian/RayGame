#include <SDL2/SDL.h>
#include <memory>
#include <cmath>
#include <iostream>
#include <sstream>
#include <unordered_map>

typedef std::unique_ptr<SDL_Renderer, decltype(SDL_DestroyRenderer)> SDLRendererPtr;
typedef std::array<std::array<char,16>,16> GameMap;

// The 6 wall textures, each 64 pixels wide, 64 pixels high total
SDL_Surface* map_textures = nullptr;

SDL_Renderer* renderer = nullptr;

int full_image_size_w = 1024;
int full_image_size_h = 512;
int map_render_size_w = 512;
int map_render_size_h = 512;
int texture_width = 64;

int to_pixels_x(const GameMap& map, SDL_Surface* surface, float map_position)
{
    // Map width in absolute coordinates
    const size_t map_w = map[0].size();

    // How large a block is in pixels assuming the map is divided evenly
    int block_size_w = (map_render_size_w / map_w);

    return  map_position * block_size_w;
}

int to_pixels_y(const GameMap& map, SDL_Surface* surface, float map_position)
{
    // Map height in absolute coordinates
    const size_t map_y = map.size();

    // How large a block is in pixels assuming the map is divided evenly
    int block_size_h = (map_render_size_h / map_y);

    return  map_position * block_size_h;
}

struct player
{
    player() : x(0), y(0), a(0), fov(M_PI/3)
    { }
    float x;
    float y;
    float a;
    float fov;
};

struct ray_cast_result
{
    ray_cast_result() : distance(0), x(0), y(0), tile_hit(' ')
    { }
    float distance;
    float x;
    float y;
    char tile_hit;
};

void init_surface(SDL_Surface* surface)
{
    for(int i = 0; i < map_render_size_h; ++i)
    for(int j = 0; j < map_render_size_w; ++j)
    {
        uint8_t r = 255*j/float(map_render_size_h); // varies between 0 and 255 as j sweeps the vertical
        uint8_t g = 255*i/float(map_render_size_w); // varies between 0 and 255 as i sweeps the horizontal
        uint8_t b = 0;        
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawPoint(renderer, i, j);
    }
}

SDL_Color get_pixel(SDL_Surface *surface, int x, int y)
{
    Uint32* pixels = (Uint32 *)surface->pixels;
    Uint32 *target_pixel = (Uint32 *)pixels + y * surface->pitch + x;
    SDL_Color color;
    SDL_GetRGBA(*target_pixel, surface->format, &color.r, &color.g, &color.b, &color.a);
    return color;
}

void draw_map(const GameMap& map, SDL_Surface* surface)
{
    const size_t map_h = map.size();
    const size_t map_w = map[0].size();
    int block_size_w = (map_render_size_w / map_w);
    int block_size_h = (map_render_size_h / map_h);

    for(int row = 0; row < map_h; ++row)
    for(int col = 0; col < map_w; ++col)
    {
        char map_i_j = map[row][col];
        if(map_i_j == ' ')
            continue;
        SDL_Rect wall { to_pixels_x(map, surface, col), to_pixels_y(map, surface, row), block_size_w, block_size_h };

        SDL_Color wall_color = get_pixel(map_textures, 64 * (map_i_j - '0'), 0);
        SDL_SetRenderDrawColor(renderer, wall_color.r, wall_color.g, wall_color.b, wall_color.a);
        SDL_RenderFillRect(renderer, &wall);
    }
}

float to_radians(float degrees)
{
    return (M_PI / 180.f) * degrees;
}

// Distance of ray and map tile hit.
ray_cast_result trace_ray(float start_x, float start_y, float ang, const GameMap& map, SDL_Surface* surface)
{
    ray_cast_result result;

    for (float c = 0; c < 20; c += .05)
    {
        float x = start_x + c * cos(ang);
        float y = start_y + c * sin(ang);
        char map_x_y = map[(int)y][(int)x];
        if (map_x_y != ' ')
        {
            result.distance = c;
            result.x = x;
            result.y = y;
            result.tile_hit = map_x_y;
            break;
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawPoint(renderer, to_pixels_x(map, surface, x), to_pixels_y(map, surface, y));
    }

    return result;
}

float trace_player_fov_rays(const player& p, const GameMap& map, SDL_Surface* surface)
{
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

    // Draw Rays on player FOV, and then 3D image on the right half of the surface
    float start_ang = p.a - p.fov / 2.0; // TODO: This probably won't always work
    float end_ang = p.a + p.fov / 2.0;
    int ray_number = 0;
    for(float current_ang = start_ang; current_ang < end_ang; current_ang += (p.fov / 512.0), ++ray_number)
    {
        ray_cast_result cast_result = trace_ray(p.x, p.y, current_ang, map, surface);
        float ray_dist = cast_result.distance;
        char tile_hit = cast_result.tile_hit;
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
        int texture_index = cast_result.tile_hit - '0';

        SDL_Rect texture_rect { (texture_width * texture_index) + textcord_x, 0, 1, texture_width };
        SDL_Rect dest_rect { column_x, column_y, column_w, column_h};
        SDL_BlitScaled(map_textures, &texture_rect, surface, &dest_rect);
    }
}

float draw_player_3d_columns(const player& p, const GameMap& map, SDL_Surface* surface)
{
    // Draw Rays on player FOV
    float start_ang = p.a - p.fov / 2.0; // TODO: This probably won't always work
    float end_ang = p.a + p.fov / 2.0;
    for(float current_ang = start_ang; current_ang < end_ang; current_ang += (p.fov / 512.0))
    {
        trace_ray(p.x, p.y, current_ang, map, surface);
    }
}

int main()
{
    // Init
    SDL_Init(SDL_INIT_EVERYTHING);

    map_textures = SDL_LoadBMP("walltext.bmp");
    if(map_textures == nullptr) {
        std::cerr << "Could not load walltext.bmp" << std::endl;
        return 1;
    }

    // Map
    GameMap map = // our game map
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


    // Player
    player p;
    p.x = 3.456;
    p.y = 2.345;
    p.a = 1.523;

    // Draw "Look around in a circle loop"
    int look_count = 0;
    for(float player_angle = 0; player_angle < 2 * M_PI; player_angle += ((2 * M_PI) / 360.0), ++look_count)
    {
        // Different surface and renderer per render
        SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, full_image_size_w, full_image_size_h, 8, SDL_PIXELFORMAT_ARGB32);
        renderer = SDL_CreateSoftwareRenderer(surface);

        // Draw Map
        draw_map(map, surface);

        // Draw Player
        SDL_Rect playerpos { to_pixels_x(map, surface, p.x), to_pixels_y(map, surface, p.y), 8, 8 };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &playerpos);

        // Adjust angle and render rays and 3D image.
        p.a = player_angle;
        trace_player_fov_rays(p, map, surface);

        // Save image of this render
        std::stringstream ss;
        ss << "output_" << look_count << ".bmp";
        std::string s = ss.str();
        SDL_SaveBMP(surface, s.c_str());

        SDL_DestroyRenderer(renderer);
        SDL_FreeSurface(surface);
    }

    // Stop
    return 0;
}