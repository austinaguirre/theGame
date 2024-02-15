//include/perlin_noise.h
#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#include <SDL.h>

float fade(float t);
float lerp(float a, float b, float t);
float grad(int hash, float x, float y);
float perlin(float x, float y);
void initPerlinNoise(unsigned int seed);

#endif
