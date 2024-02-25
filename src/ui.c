// ui.c
#include "ui.h"


void renderButton(SDL_Renderer* renderer, UIButton* button) {
    // Set button color
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Example color

    SDL_Rect buttonRect = {button->x, button->y, button->width, button->height};
    SDL_RenderFillRect(renderer, &buttonRect);

    // Render button label text
    render_text(renderer, button->label, button->x + 5, button->y + 5, textColor, textScale * 0.5);
}

