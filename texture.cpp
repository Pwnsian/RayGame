#include "texture.h"
#include <sstream>
#include <iostream>

texture_manager::texture_manager(const std::string& source_directory)
    : source_directory(source_directory)
{
    textures[texture_name::TEXTURES_WALL] = load_texture("walltext.bmp");
    textures[texture_name::TEXTURES_MONSTERS] = load_texture("monsters.bmp");    
}

SDL_Surface* texture_manager::load_texture(const std::string& name)
{
    std::stringstream ss;
    ss << source_directory << "/" << name;
    SDL_Surface* tex = SDL_LoadBMP(ss.str().c_str());
    if(tex == nullptr) {
        std::cerr << "Could not load " << ss.str() << std::endl;
    }
    return tex;
}

SDL_Color texture_manager::get_pixel(texture_name name, int x, int y)
{
    SDL_Surface* texture = get_texture(name);

    Uint32* pixels = (Uint32 *)texture->pixels;
    Uint32 *target_pixel = (Uint32 *)pixels + y * texture->pitch + x;
    SDL_Color color;
    SDL_GetRGBA(*target_pixel, texture->format, &color.r, &color.g, &color.b, &color.a);

    return color;
}

SDL_Surface* texture_manager::get_texture(texture_name name)
{
    return textures[name];
}
