// src/camera.c
#include "config.h"
#include "camera.h"
#include "world.h"
#include "player.h"


void update_camera_position(Camera* camera, const World* world, const Player* player) {
    // Center the camera on the player
    camera->x = player->x - camera->width / 2;
    camera->y = player->y - camera->height / 2;

    // Clamp the camera to the world bounds
    if (camera->x < 0) camera->x = 0;
    if (camera->y < 0) camera->y = 0;
    if (camera->x + camera->width > world->width * TILE_SIZE) camera->x = world->width * TILE_SIZE - camera->width;
    if (camera->y + camera->height > world->height * TILE_SIZE) camera->y = world->height * TILE_SIZE - camera->height;
}





