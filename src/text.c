#include "text.h"
#include <string.h>
#include <SDL_image.h>

static SDL_Texture* fontTexture = NULL;
static SDL_Rect characterMap[128];
static int charWidth = 80; // Width of each character in the texture
static int charHeight = 80; // Height of each character

void text_init(SDL_Renderer* renderer, const char* fontPath) {
    fontTexture = IMG_LoadTexture(renderer, fontPath);
    if (!fontTexture) {
        printf("Failed to load font texture\n");
        return;
    }

    // Assuming 'a' starts at 0,0 and follows ASCII order for a-z, 0-9, and special chars
    for (int i = 'a'; i <= 'z'; i++) { // Map a-z
        int idx = i - 'a';
        characterMap[i].x = (idx % 8) * charWidth;
        characterMap[i].y = (idx / 8) * charHeight;
        characterMap[i].w = charWidth;
        characterMap[i].h = charHeight;
    }
    for (int i = '0'; i <= '9'; i++) { // Map 0-9
        int idx = 26 + i - '0'; // Continue after 'z'
        characterMap[i].x = (idx % 8) * charWidth;
        characterMap[i].y = (idx / 8) * charHeight;
        characterMap[i].w = charWidth;
        characterMap[i].h = charHeight;
    }

    // Manually map special characters
    const char specialChars[] = "$:?!";
    for (size_t i = 0; i < strlen(specialChars); i++) {
        int idx = 36 + i; // Continue after '9'
        characterMap[(int)specialChars[i]].x = (idx % 8) * charWidth;
        characterMap[(int)specialChars[i]].y = (idx / 8) * charHeight;
        characterMap[(int)specialChars[i]].w = charWidth;
        characterMap[(int)specialChars[i]].h = charHeight;
    }
}

void render_text_trimmed(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color, float scale) {
    SDL_SetTextureColorMod(fontTexture, color.r, color.g, color.b);
    int cursorX = x;

    // Use trimSize to control how much to trim from each side
    int trimSizeX = 20 + 4;
    int trimSizeY = 20; // Use this if you want to trim top and bottom as well

    for (const char* c = text; *c; c++) {
        if (*c == ' ') { // Handle spaces by advancing the cursor
            cursorX += (charWidth - (2 * trimSizeX)) * scale; // Adjust space width for trimmed characters
            continue;
        }
        if ((unsigned char)*c >= 32 && (unsigned char)*c < 128) {
            SDL_Rect srcRect = characterMap[(unsigned char)*c];

            // Adjust srcRect to trim whitespace around characters
            srcRect.x += trimSizeX;
            srcRect.y += trimSizeY; // Adjust if trimming top and bottom
            srcRect.w -= 2 * trimSizeX; // Reduce width to account for trimming on both sides
            srcRect.h -= 2 * trimSizeY; // Reduce height to account for trimming on top and bottom

            SDL_Rect destRect = {
                cursorX,
                y + (trimSizeY * scale), // Adjust Y position if trimming top
                (int)(srcRect.w * scale),
                (int)(srcRect.h * scale)
            };
            SDL_RenderCopy(renderer, fontTexture, &srcRect, &destRect);
            cursorX += destRect.w + (5 * scale); // Advance cursor by scaled width of the character + additional spacing
        }
    }
}


void render_text(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color, float scale) {
    SDL_SetTextureColorMod(fontTexture, color.r, color.g, color.b);
    int cursorX = x;

    for (const char* c = text; *c; c++) {
        if (*c == ' ') { // Handle spaces by advancing the cursor
            cursorX += charWidth * scale; // Adjust as needed
            continue;
        }
        if ((unsigned char)*c >= 32 && (unsigned char)*c < 128) {
            SDL_Rect srcRect = characterMap[(unsigned char)*c];
            // Adjust srcRect here if necessary to reduce white space
            
            SDL_Rect destRect = {cursorX, y, (int)(srcRect.w * scale), (int)(srcRect.h * scale)};
            SDL_RenderCopy(renderer, fontTexture, &srcRect, &destRect);
            cursorX += destRect.w; // Advance cursor by scaled width of the character
        }
    }
}


void text_cleanup(void) {
    if (fontTexture) {
        SDL_DestroyTexture(fontTexture);
        fontTexture = NULL;
    }
}

