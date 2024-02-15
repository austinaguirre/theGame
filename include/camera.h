// include/camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include <SDL.h>
#include "world.h"
#include "player.h"
// struct World;
// struct Player;

typedef struct {
    int x, y; // Camera's top-left corner in the world
    int width, height; // Camera's size
} Camera;

void update_camera_position(Camera* camera, const World* world, const Player* player);

#endif
