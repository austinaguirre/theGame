// assets/text/player.png

#include "text.h"
#include "graphics.h" // For `load_texture`

// Assuming these are defined or included from elsewhere
#define FONT_TEXTURE_PATH "assets/text/alphabet.png"
#define CHAR_WIDTH 160
#define CHAR_HEIGHT 160
#define COLUMNS 8
#define ROWS 5

static SDL_Rect characterMap[128]; // Assuming ASCII range for simplicity
static SDL_Texture* fontTexture = NULL;

void text_init(SDL_Renderer* renderer) {
    // Load the sprite font texture
    fontTexture = load_texture(renderer, FONT_TEXTURE_PATH);
    
    // Initialize the character map
    for (int i = 0; i < ROWS * COLUMNS; i++) {
        characterMap[i].x = (i % COLUMNS) * CHAR_WIDTH;
        characterMap[i].y = (i / COLUMNS) * CHAR_HEIGHT;
        characterMap[i].w = CHAR_WIDTH;
        characterMap[i].h = CHAR_HEIGHT;
    }

    // Custom mapping for characters a-z, 0-9, and symbols based on their order in the sprite
    const char* charOrder = "abcdefghijklmnopqrstuvwxyz0123456789$:?!";
    for (int i = 0; charOrder[i] != '\0'; i++) {
        characterMap[(int)charOrder[i]] = characterMap[i];
    }
}

void render_text(SDL_Renderer* renderer, const char* text, int x, int y) {
    for (const char* c = text; *c; c++) {
        if (*c >= ' ' && *c <= '~') { // Basic printable ASCII
            SDL_Rect srcRect = characterMap[(int)*c];
            SDL_Rect destRect = {x, y, srcRect.w, srcRect.h};
            SDL_RenderCopy(renderer, fontTexture, &srcRect, &destRect);
            x += srcRect.w; // Move cursor for the next character
        }
    }
}

void text_cleanup(void) {
    if (fontTexture) {
        SDL_DestroyTexture(fontTexture);
        fontTexture = NULL;
    }
}

