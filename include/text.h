// include/text.h
#ifndef TEXT_H
#define TEXT_H

#include <SDL.h>

// Function to initialize the text rendering system
void text_init(SDL_Renderer* renderer);

// Function to render text
void render_text(SDL_Renderer* renderer, const char* text, int x, int y);

// Function to clean up resources used by the text rendering system
void text_cleanup(void);

#endif
