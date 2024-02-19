// ui.h
#ifndef UI_H
#define UI_H

#include <SDL.h>
#include <stdbool.h>

typedef struct {
    int x, y; // Position
    int width, height; // Dimensions
    char* label; // Optional: For button labels or other text
    // You can expand this struct with more properties, like textures for the button.
} UIButton;

// Function prototypes

#endif // UI_H
