// include/graphics.h
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL.h>

// Initializes the SDL graphics subsystem and creates the window and renderer
void graphics_init(SDL_Window** window, SDL_Renderer** renderer);

// Loads a texture from a file
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* filePath);

// Renders a texture at a given position
void render_texture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y);

#endif
