#include <SDL2/SDL.h>

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

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 512, 512, 8, SDL_PIXELFORMAT_ARGB32);
    init_surface(surface);
    SDL_SaveBMP(surface, "output.bmp");
    return 0;
}