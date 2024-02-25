// ui.h
#ifndef UI_H
#define UI_H

#include <SDL.h>
#include <stdbool.h>
#include "config.h"
#include "text.h"

typedef struct {
    int x, y; // Position
    int width, height; // Dimensions
    char* label; // Optional: For button labels or other text
    // You can expand this struct with more properties, like textures for the button.
} UIButton;

void renderButton(SDL_Renderer* renderer, UIButton* button);

#endif // UI_H
