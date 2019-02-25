#ifndef TEXTURES_H
#define TEXTURES_H

#include <SDL2/SDL.h>
#include <unordered_map>

enum class texture_type : char
{
    TEXTURE_REDSTONE = '0',
    TEXTURE_STONE_WALL = '1',
    TEXTURE_BURNT_WALL = '2',
    TEXTURE_WOOD_DOOR = '3',
    TEXTURE_GREEN_METAL = '4',
    TEXTURE_MOSS_STONE = '5',
    TEXTURE_NONE = ' '
};

enum class texture_name : int
{
    TEXTURES_WALL = 0
};

class texture_manager
{
public:
    texture_manager(const std::string& source_directory);
    SDL_Color get_pixel(texture_name name, int x, int y);
    SDL_Surface* get_texture(texture_name name);

private:
    SDL_Surface* load_texture(const std::string& name);
    std::string source_directory;
    std::unordered_map<texture_name, SDL_Surface*> textures;
};

#endif
