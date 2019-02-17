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

struct player
{
    player() : x(0), y(0), a(0)
    { }
    float x;
    float y;
    float a;
};

void init_surface(SDL_Surface* surface)
{
    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);
    for(int i = 0; i < surface->h; ++i)
    for(int j = 0; j < surface->w; ++j)
    {
        uint8_t r = 255*j/float(surface->h); // varies between 0 and 255 as j sweeps the vertical
        uint8_t g = 255*i/float(surface->w); // varies between 0 and 255 as i sweeps the horizontal
        uint8_t b = 0;        
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawPoint(renderer, i, j);
    }
    SDL_DestroyRenderer(renderer);
}

void draw_map(const GameMap& map, SDL_Surface* surface)
{
    const size_t map_w = map.size();
    const size_t map_h = map[0].size();

    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);
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
    SDL_DestroyRenderer(renderer);    
}

float to_radians(float degrees)
{
    return (M_PI / 180.f) * degrees;
}

float trace_player_ray(const player& player, const GameMap& map, SDL_Surface* surface)
{
   float c = 0;   
    for (; c<20; c+=.05) {
        float x = player.x + c* cos(player.a);
        float y = player.y + c* sin(player.a);
        if (map[y][x]!=' ') break;
   }
   return c;
}

int main()
{
    // Init
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 512, 512, 8, SDL_PIXELFORMAT_ARGB32);
    init_surface(surface);

    //GameMap map = {1,1};

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
    p.x = 50;
    p.y = 50;
    p.a = to_radians(30);
    float dist = trace_player_ray(p, map, surface);
    std::cout << dist << std::endl;

    // Stop
    SDL_SaveBMP(surface, "output.bmp");
    return 0;
}