#pragma once

#include <math.h>
#include <stdlib.h>
#include <stdarg.h>

#define FADE(t) ( t * t * t * ( t * ( t * 6 - 15 ) + 10 ) )
#define FASTFLOOR(x) ( ((int)(x)<(x)) ? ((int)x) : ((int)x-1 ) )
#define LERP(t, a, b) ((a) + (t)*((b)-(a)))

int *shuffled_permutations(int size);
float noise2(float x, float y, int *perm);
float snoise3(float x, float y, float z, int *perm);
float noise_layered(int count, ...);
float fractal2(float x, float y, int *permutations);
float fractal3(float x, float y, float z, int *permutations);
