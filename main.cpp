#include <SDL2/SDL.h>
#include <memory>
#include <cmath>
#include <iostream>

typedef std::unique_ptr<SDL_Renderer, decltype(SDL_DestroyRenderer)> SDLRendererPtr;
typedef std::array<std::array<char,16>,16> GameMap;

//SDLRendererPtr MakeSDLRenderer(SDL_Surface* surface)
//{
//    return SDLRendererPtr(SDL_CreateSoftwareRenderer(surface), SDL_DestroyRenderer);
//}

SDL_Renderer* renderer = nullptr;

struct player
{
    player() : x(0), y(0), a(0), fov(M_PI/3)
    { }
    float x;
    float y;
    float a;
    float fov;
};

void init_surface(SDL_Surface* surface)
{
    for(int i = 0; i < surface->h; ++i)
    for(int j = 0; j < surface->w; ++j)
    {
        uint8_t r = 255*j/float(surface->h); // varies between 0 and 255 as j sweeps the vertical
        uint8_t g = 255*i/float(surface->w); // varies between 0 and 255 as i sweeps the horizontal
        uint8_t b = 0;        
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawPoint(renderer, i, j);
    }
}

void draw_map(const GameMap& map, SDL_Surface* surface)
{
    const size_t map_h = map.size();
    const size_t map_w = map[0].size();

    int block_size_w = (surface->w / map_w);
    int block_size_h = (surface->h / map_h);
    for(int row = 0; row < map_h; ++row)
    for(int col = 0; col < map_w; ++col)
    {
        char map_i_j = map[row][col];
        if(map_i_j == ' ')
            continue;
        int block_x = block_size_w * col;            
        int block_y = block_size_h * row;
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);   
        SDL_Rect wall { block_x, block_y, block_size_w, block_size_h };
        SDL_RenderFillRect(renderer, &wall);
    }   
}

float to_radians(float degrees)
{
    return (M_PI / 180.f) * degrees;
}

int to_pixels_x(const GameMap& map, SDL_Surface* surface, float map_position)
{
    // Map width in absolute coordinates
    const size_t map_w = map[0].size();

    // How large a block is in pixels assuming the map is divided evenly
    int block_size_w = (surface->w / map_w);

    return  map_position * block_size_w;
}

int to_pixels_y(const GameMap& map, SDL_Surface* surface, float map_position)
{
    // Map height in absolute coordinates
    const size_t map_y = map.size();

    // How large a block is in pixels assuming the map is divided evenly
    int block_size_h = (surface->h / map_y);

    return  map_position * block_size_h;
}

float trace_ray(float start_x, float start_y, float ang, const GameMap& map, SDL_Surface* surface)
{
    float c = 0;
    for (; c < 20; c += .05) 
    {
        float x = start_x + c * cos(ang);
        float y = start_y + c * sin(ang);
        if (map[(int)y][(int)x] != ' ')
            break;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);            
        SDL_RenderDrawPoint(renderer, to_pixels_x(map, surface, x), to_pixels_y(map, surface, y));
    }

    return c;
}

float trace_player_fov_rays(const player& p, const GameMap& map, SDL_Surface* surface)
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
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 512, 512, 8, SDL_PIXELFORMAT_ARGB32);
    renderer = SDL_CreateSoftwareRenderer(surface);    
    init_surface(surface);      

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
        {'0',' ',' ',' ','0',' ',' ',' ','0',' ',' ',' ',' ',' ',' ','0'},
        {'0',' ',' ',' ','0',' ',' ',' ','1',' ',' ','0','0','0','0','0'},
        {'0',' ',' ',' ',' ',' ',' ',' ','1',' ',' ',' ',' ',' ',' ','0'},
        {'2',' ',' ',' ',' ',' ',' ',' ','1',' ',' ',' ',' ',' ',' ','0'},
        {'0',' ',' ',' ',' ',' ',' ',' ','0',' ',' ',' ',' ',' ',' ','0'},
        {'0',' ','0','0','0','0','0','0','0',' ',' ',' ',' ',' ',' ','0'},
        {'0',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','0'},
        {'0','0','0','2','2','2','2','2','2','2','2','0','0','0','0','0'}
    }};
    draw_map(map, surface);

    // Player
    player p;
    p.x = 3.456;
    p.y = 2.345;
    p.a = 1.523;
    SDL_Rect playerpos { to_pixels_x(map, surface, p.x), to_pixels_y(map, surface, p.y), 8, 8 };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &playerpos);
    trace_player_fov_rays(p, map, surface);


    // Stop
    SDL_SaveBMP(surface, "output.bmp");
    SDL_DestroyRenderer(renderer);  
    return 0;
}