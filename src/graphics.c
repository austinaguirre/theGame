// src/graphics.c
#include "graphics.h"
#include "config.h"
#include <SDL_image.h>
#include "world.h"
#include "player.h"
#include "camera.h"

void graphics_init(SDL_Window** window, SDL_Renderer** renderer) {
    SDL_Init(SDL_INIT_VIDEO);
    *window = SDL_CreateWindow("2D Adventure Game", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
}

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* filePath) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, filePath);
    if (!texture) {
        printf("Failed to load texture: %s\n", IMG_GetError());
    }
    return texture;
}

void render_texture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y) {
    SDL_Rect dest = {x, y, 50, 50}; // Modify as needed
    SDL_RenderCopy(renderer, texture, NULL, &dest);
}

void player_render(Player* player, SDL_Renderer* renderer, const Camera* camera) {
    // Calculate the player's position relative to the camera's viewport
    SDL_Rect dstRect = {
        (player->x * CELL_WIDTH) - camera->x, // Adjust player's X position by camera offset
        (player->y * CELL_HEIGHT) - camera->y, // Adjust player's Y position by camera offset
        CELL_WIDTH,
        CELL_HEIGHT
    };
    SDL_RenderCopy(renderer, player->texture, NULL, &dstRect);
}

SDL_Color getColorForTile(TerrainType terrain, WaterBodyType waterBody, SpecialFeatureType specialFeature, StructureType structure, TerrainHeight height, TileRenderCount* count) {
    SDL_Color c = {220, 220, 220, 255}; // Default color (Gainsboro)

    count->terrainCounts[terrain]++;
    count->waterBodyCounts[waterBody]++;
    count->specialFeatureCounts[specialFeature]++;
    count->structureCounts[structure]++;
    count->heightCounts[height]++;

    // Switch for terrain types
    switch (terrain) {
        case TITLE_VOLCANIC_LANDS: c.r = 255; c.g = 69; c.b = 0; c.a = 255; break; // Orange for volcanic lands
        case TITLE_TUNDRA: c.r = 176; c.g = 224; c.b = 230; c.a = 255; break; // Powder Blue for tundra
        case TITLE_GRASSLANDS: c.r = 124; c.g = 252; c.b = 0; c.a = 255; break; // Lawn Green for grasslands
        case TITLE_SWAMPLAND: c.r = 47; c.g = 79; c.b = 79; c.a = 255; break; // Dark Slate Grey for swampland
        case TITLE_DEAD_LANDS: c.r = 170; c.g = 170; c.b = 170; c.a = 255; break; // Dim Grey for dead lands
        case TITLE_FOREST: c.r = 34; c.g = 139; c.b = 34; c.a = 255; break; // Forest Green for forest
        case TITLE_SPIDERLANDS: c.r = 30; c.g = 30; c.b = 30; c.a = 255; break; // Near Black for spiderlands
        case TITLE_MOUNTAIN: c.r = 139; c.g = 137; c.b = 137; c.a = 255; break; // Grey for mountain
        case TITLE_SCORCHED_LANDS: c.r = 210; c.g = 180; c.b = 140; c.a = 255; break; // Tan for scorched lands
        case TITLE_SAVANNAH: c.r = 238; c.g = 232; c.b = 170; c.a = 255; break; // Pale Goldenrod for savannah
        case TITLE_JUNGLE: c.r = 0; c.g = 100; c.b = 0; c.a = 255; break; // Dark Green for jungle
        case TITLE_BEACH: c.r = 238; c.g = 214; c.b = 175; c.a = 255; break; // Bisque for beach
        case TITLE_DESERT: c.r = 244; c.g = 164; c.b = 96; c.a = 255; break; // Sandy Brown for desert
        case TITLE_DEFAULT_TERRAIN: c.r = 220; c.g = 220; c.b = 220; c.a = 255; break; // Gainsboro for default terrain
        // Default color if not specified
        default: c.r = 220; c.g = 220; c.b = 220; c.a = 255; break;
    }

    // Switch for water body types
    switch (waterBody) {
        // Determine the color based on the WaterBodyType
        case TITLE_RIVER: c.r = 30; c.g = 144; c.b = 255; c.a = 255; break; // Dodger Blue for rivers
        case TITLE_OCEAN: c.r = 0; c.g = 105; c.b = 148; c.a = 255; break; // Deep Sky Blue for oceans
        case TITLE_DEEP_OCEAN: c.r = 0; c.g = 0; c.b = 139; c.a = 255; break; // Dark Blue for deep oceans
        case TITLE_LAKE: c.r = 173; c.g = 216; c.b = 230; c.a = 255; break; // Light Blue for lakes
        case TITLE_DEFAULT_WATER: break; // No change for default water type to allow terrain color
        default: break; // No override if not a recognized water body type
    }

    // Switch for special features
    switch (specialFeature) {
        case TITLE_BONE_BEACH: c.r = 237; c.g = 228; c.b = 211; c.a = 255; break; // Light gray for Bone Beach
        case TITLE_TROPICAL_BEACH: c.r = 201; c.g = 235; c.b = 195; c.a = 255; break; // some green ish i chose 201, 235, 195
        case TITLE_SNOWY_MOUNTAIN: c.r = 255; c.g = 250; c.b = 250; c.a = 255; break; // Snow for Snowy Mountain
        case TITLE_DEEP_OCEAN_TRENCH: c.r = 0; c.g = 0; c.b = 0; c.a = 255; break; // Midnight Blue for Deep Ocean Trench
        case TITLE_CORAL_REEF: c.r = 255; c.g = 127; c.b = 80; c.a = 255; break; // Coral color for Coral Reef
        case TITLE_CRYSTAL_CAVE: c.r = 135; c.g = 206; c.b = 250; c.a = 255; break; // Light Sky Blue for Crystal Cave
        case TITLE_PARADISE_ISLES: c.r = 64; c.g = 224; c.b = 208; c.a = 255; break; // Turquoise for Paradise Isles
        case TITLE_ICY_TUNDRA: c.r = 65; c.g = 255; c.b = 245; c.a = 255; break; // Powder Blue for Icy Tundra
        case TITLE_SULFURIC_SPRINGS: c.r = 154; c.g = 205; c.b = 50; c.a = 255; break; // Yellow Green for Sulfuric Springs
        case TITLE_CRYSTAL_FOREST: c.r = 72; c.g = 61; c.b = 139; c.a = 255; break; // Dark Slate Blue for Crystal Forest
        case TITLE_BAMBOO_FOREST: c.r = 107; c.g = 142; c.b = 35; c.a = 255; break; // Olive Drab for Bamboo Forest
        case TITLE_IRONWOOD_FOREST: c.r = 139; c.g = 69; c.b = 19; c.a = 225; break; // Saddle Brown for Ironwood Forest
        case TITLE_FUNGI_FOREST: c.r = 255; c.g = 0; c.b = 255; c.a = 255; break; // Magenta for Fungi Forest
        case TITLE_REDWOOD_FOREST: c.r = 165; c.g = 42; c.b = 42; c.a = 255; break; // Brown for Redwood Forest
        case TITLE_BLOSSOM_FOREST: c.r = 255; c.g = 182; c.b = 193; c.a = 255; break; // Light Pink for Blossom Forest
        case TITLE_CANYON: c.r = 124; c.g = 252; c.b = 0; c.a = 255; break; // Chocolate for Canyon
        case TITLE_SMOOTH_GRASSLANDS_GREEN_HILLS: c.r = 16; c.g = 252; c.b = 0; c.a = 255; break; // Lawn Green for Smooth Grasslands
        case TITLE_ACTIVE_VOLCANIC_LANDS: c.r = 178; c.g = 34; c.b = 34; c.a = 255; break; // Firebrick for Active Volcanic Lands
        case TITLE_DEFAULT_SPECIAL: break; // No change for default special feature to allow terrain or water body color
        default: break; // No override if not a recognized special feature type
    }

    // Switch for structure types
    switch (structure) {
        case TITLE_SHIP_WRECK: c.r = 105; c.g = 105; c.b = 105; c.a = 255; break; // Dim Grey for Ship Wreck
        case TITLE_FARMLAND: c.r = 245; c.g = 222; c.b = 179; c.a = 255; break; // Wheat color for Farmland
        case TITLE_CAVE: c.r = 112; c.g = 128; c.b = 144; c.a = 255; break; // Slate Grey for Cave
        case TITLE_ANCIENT_RUINS: c.r = 184; c.g = 134; c.b = 11; c.a = 255; break; // Dark Golden Rod for Ancient Ruins
        case TITLE_DOCK: c.r = 139; c.g = 69; c.b = 19; c.a = 255; break; // Saddle Brown for Dock
        case TITLE_CITY_YANGSE: c.r = 169; c.g = 169; c.b = 169; c.a = 255; break; // Dark Gray for City Yangse
        case TITLE_CITY_RAMONDULL: c.r = 169; c.g = 169; c.b = 169; c.a = 255; break; // Similar Dark Gray for City Ramondull
        case TITLE_CITY_MYKE: c.r = 169; c.g = 169; c.b = 169; c.a = 255; break; // Keeping cities with the same color scheme
        case TITLE_CITY_DORPORT: c.r = 169; c.g = 169; c.b = 169; c.a = 255; break; // Dark Gray for City Dorport
        case TITLE_ORC_CAMP: c.r = 128; c.g = 0; c.b = 0; c.a = 255; break; // Maroon for Orc Camp
        case TITLE_ORC_CASTLE: c.r = 139; c.g = 0; c.b = 0; c.a = 255; break; // Dark Red for Orc Castle
        case TITLE_ELF_HIDEOUT: c.r = 34; c.g = 139; c.b = 34; c.a = 255; break; // Forest Green for Elf Hideout
        case TITLE_DEFAULT_STRUCTURE: break; // No change for default structure to allow terrain or special feature color
        default: break; // No override if not a recognized structure type
    }

    return c;
}

const char* HeightTypeNames[] = {
    "Very Low", "Low", "Medium", "High", "Very High"
};

// Example arrays for enum to string mapping
const char* TerrainTypeNames[] = {
    "Volcanic Lands", "Tundra", "Grasslands", "Swampland", "Dead Lands",
    "Forest", "Spiderlands", "Mountain", "Scorched Lands", "Savannah",
    "Jungle", "Beach", "Desert", "Default Terrain"
};

const char* WaterBodyTypeNames[] = {
    "River", "Ocean", "Deep Ocean", "Lake", "Default Water"
};

const char* SpecialFeatureTypeNames[] = {
    "Bone Beach", "Tropical Beach", "Snowy Mountain", "Deep Ocean Trench",
    "Coral Reef", "Crystal Cave", "Paradise Isles", "Icy Tundra",
    "Sulfuric Springs", "Crystal Forest", "Bamboo Forest", "Ironwood Forest",
    "Fungi Forest", "Redwood Forest", "Blossom Forest", "Canyon",
    "Smooth Grasslands Green Hills", "Active Volcanic Lands", "Default Special"
};

const char* StructureTypeNames[] = {
    "Ship Wreck", "Farmland", "Cave", "Ancient Ruins", "Dock",
    "City Yangse", "City Ramondull", "City Myke", "City Dorport",
    "Orc Camp", "Orc Castle", "Elf Hideout", "Default Structure"
};

void printDebugInfo(const TileRenderCount* count) {
    printf("-----------------------------------\n");
    printf("Height Types Render Count:\n");
    for (int i = 0; i < sizeof(HeightTypeNames)/sizeof(HeightTypeNames[0]); i++) {
        printf("%s rendered %d times\n", HeightTypeNames[i], count->heightCounts[i]);
    }
    printf("\nTerrain Types Render Count:\n");
    for (int i = 0; i < sizeof(TerrainTypeNames)/sizeof(TerrainTypeNames[0]); i++) {
        printf("%s rendered %d times\n", TerrainTypeNames[i], count->terrainCounts[i]);
    }

    printf("\nWater Body Types Render Count:\n");
    for (int i = 0; i < sizeof(WaterBodyTypeNames)/sizeof(WaterBodyTypeNames[0]); i++) {
        printf("%s rendered %d times\n", WaterBodyTypeNames[i], count->waterBodyCounts[i]);
    }

    printf("\nSpecial Feature Types Render Count:\n");
    for (int i = 0; i < sizeof(SpecialFeatureTypeNames)/sizeof(SpecialFeatureTypeNames[0]); i++) {
        printf("%s rendered %d times\n", SpecialFeatureTypeNames[i], count->specialFeatureCounts[i]);
    }

    printf("\nStructure Types Render Count:\n");
    for (int i = 0; i < sizeof(StructureTypeNames)/sizeof(StructureTypeNames[0]); i++) {
        printf("%s rendered %d times\n", StructureTypeNames[i], count->structureCounts[i]);
    }
}


void world_render(const World* world, SDL_Renderer* renderer, const Camera* camera) {
    // Adjusted calculation for start and end tiles
    int startx = max(camera->x / TILE_SIZE - 1, 0);
    int endx = min((camera->x + camera->width) / TILE_SIZE + 1, world->width);
    int starty = max(camera->y / TILE_SIZE - 1, 0);
    int endy = min((camera->y + camera->height) / TILE_SIZE + 1, world->height);

    TileRenderCount tileRenderCount = {0};

    for (int y = starty; y < endy; y++) {
        for (int x = startx; x < endx; x++) {
            SDL_Rect tile = {(x * TILE_SIZE) - camera->x, (y * TILE_SIZE) - camera->y, TILE_SIZE, TILE_SIZE};

            SDL_Color c = getColorForTile(world->map[y][x].terrain, world->map[y][x].waterBody, world->map[y][x].specialFeature, world->map[y][x].structure, world->map[y][x].height, &tileRenderCount);

            SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
            SDL_RenderFillRect(renderer, &tile);
        }
    }
}

void renderMiniMap(SDL_Renderer* renderer, World* world, Player* player, float scaleFactor) {
    int tileWidth = TILE_SIZE * scaleFactor;
    int tileHeight = TILE_SIZE * scaleFactor;

    TileRenderCount tileRenderCount = {0};

    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            SDL_Rect tileRect = {x * tileWidth, y * tileHeight, tileWidth, tileHeight};

            SDL_Color c = getColorForTile(world->map[y][x].terrain, world->map[y][x].waterBody, world->map[y][x].specialFeature, world->map[y][x].structure, world->map[y][x].height, &tileRenderCount);

            // Determine the color based on the tile's type, simplified here for brevity
            SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a); // Example: set color to green for grass
            SDL_RenderFillRect(renderer, &tileRect);
        }
    }
    printDebugInfo(&tileRenderCount);

    // Render the player's position on the minimap
    SDL_Rect playerRect = {
        (int)(player->x * scaleFactor * TILE_SIZE),
        (int)(player->y * scaleFactor * TILE_SIZE),
        max(1, tileWidth), // Ensure the player marker is visible
        max(1, tileHeight)
    };
    int playerTileX = player->x;
    int playerTileY = player->y;
    // Ensure the player's position is within the world bounds before accessing the map array
if (playerTileX >= 0 && playerTileX < world->width && playerTileY >= 0 && playerTileY < world->height) {
    Tile playerTile = world->map[playerTileY][playerTileX];
    printf("Player is on tile at (%d, %d) with Height: %s, Terrain: %s, WaterBody: %s, SpecialFeature: %s, Structure: %s\n",
           playerTileX, playerTileY,
           HeightTypeNames[playerTile.height],
           TerrainTypeNames[playerTile.terrain],
           WaterBodyTypeNames[playerTile.waterBody],
           SpecialFeatureTypeNames[playerTile.specialFeature],
           StructureTypeNames[playerTile.structure]);
}
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for the player
    SDL_RenderFillRect(renderer, &playerRect);
}



