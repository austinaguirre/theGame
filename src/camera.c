// src/camera.c
#include "config.h"
#include "camera.h"
#include "world.h"
#include "player.h"


void update_camera_position(Camera* camera, const World* world, const Player* player) {
    // Convert player position from tiles to pixels
    int playerPixelX = player->x * TILE_SIZE;
    int playerPixelY = player->y * TILE_SIZE;

    // Center the camera on the player in pixel coordinates
    camera->x = playerPixelX - camera->width / 2;
    camera->y = playerPixelY - camera->height / 2;

    // Clamp the camera to ensure it doesn't show areas outside the world
    if (camera->x < 0) camera->x = 0;
    if (camera->y < 0) camera->y = 0;
    if (camera->x + camera->width > world->width * TILE_SIZE) camera->x = world->width * TILE_SIZE - camera->width;
    if (camera->y + camera->height > world->height * TILE_SIZE) camera->y = world->height * TILE_SIZE - camera->height;

    // Adjust for potential half-tile issues by checking if the camera is close to the world's edge
    if (camera->x + camera->width > (world->width * TILE_SIZE) - (TILE_SIZE / 2)) {
        camera->x = (world->width * TILE_SIZE) - camera->width;
    }
    if (camera->y + camera->height > (world->height * TILE_SIZE) - (TILE_SIZE / 2)) {
        camera->y = (world->height * TILE_SIZE) - camera->height;
    }
}



