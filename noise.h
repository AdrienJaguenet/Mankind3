#pragma once

#define FASTFLOOR(x) ( ((int)(x)<=(x)) ? ((int)x) : (((int)x)-1) )

int *shuffled_permutations(int size);
float snoise2(float x, float y, int *perm);
float snoise3(float x, float y, float z, int *perm);
float noise_layered(int count, ...);
float fractal2(float x, float y, int *permutations);
float fractal3(float x, float y, float z, int *permutations);
