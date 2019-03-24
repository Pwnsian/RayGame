#include <SDL2/SDL.h>
#include <sstream>
#include <vector>
#include "render.h"
#include "player.h"
#include "sprite.h"
#include "constants.h"

int main()
{
    // Init
    SDL_Init(SDL_INIT_EVERYTHING);

    // Player
    player p;
    p.x = 3.456;
    p.y = 2.345;
    p.a = 1.523;
    
    // Monsters
    std::vector<Sprite> sprites{ {1.834, 8.765, 0}, {5.323, 5.365, 1}, {4.123, 10.265, 1} };

    texture_manager texture_manager(".");
    game_map map(&texture_manager);
    renderer render(&texture_manager);

#if 0
    // Draw "Look around in a circle" loop
    int look_count = 0;
    for(float player_angle = 0; player_angle < M_PI_2; player_angle += ((2 * M_PI) / 360.0), ++look_count)
#else
    float player_angle = p.a;
    int look_count = 0;
#endif
    {
        // Different surface and renderer per render
        SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, full_image_size_w, full_image_size_h, 8, SDL_PIXELFORMAT_ARGB32);
        SDL_Renderer* sdl_renderer = SDL_CreateSoftwareRenderer(surface);

        // Draw Map
        p.a = player_angle;
        render.render(surface, sdl_renderer, map, p, sprites);

        // Save image of this render
        std::stringstream ss;
        ss << "output_" << look_count << ".bmp";
        std::string s = ss.str();
        SDL_SaveBMP(surface, s.c_str());

        SDL_DestroyRenderer(sdl_renderer);
        SDL_FreeSurface(surface);
    }

    // Stop
    return 0;
}