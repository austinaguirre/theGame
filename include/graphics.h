// include/graphics.h
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL.h>
#include "player.h"
#include "world.h"
#include "camera.h"
#include "ui.h"

// Initializes the SDL graphics subsystem and creates the window and renderer
void graphics_init(SDL_Window** window, SDL_Renderer** renderer);

// Loads a texture from a file
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* filePath);

// Renders a texture at a given position
void render_texture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y);

SDL_Color getColorForTile(TerrainType terrain, WaterBodyType waterBody, SpecialFeatureType specialFeature, StructureType structure, TerrainHeight height, TileRenderCount* count);
void player_render(Player* player, SDL_Renderer* renderer, const Camera* camera);
void world_render(const World* world, SDL_Renderer* renderer, const Camera* camera);
void renderMiniMap(SDL_Renderer* renderer, World* world, Player* player, float scaleFactor);
void printDebugInfo(const TileRenderCount* count);

void renderUI(SDL_Renderer* renderer, UIButton* button);



#endif
