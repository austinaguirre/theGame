#ifndef TEXT_H
#define TEXT_H

#include <SDL.h>

// Initialize text rendering system
void text_init(SDL_Renderer* renderer, const char* fontPath);

void render_text_trimmed(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color, float scale);

// Render text with specified color and scale
void render_text(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color, float scale);

// Clean up text rendering system
void text_cleanup(void);

char* strdupToLower(const char* str);

#endif
