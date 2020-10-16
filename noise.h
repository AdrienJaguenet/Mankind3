#pragma once

#include <math.h>
#include <stdlib.h>
#include <stdarg.h>

#include "utilities.h"

u_int8_t *shuffled_permutations(size_t size);
float noise2(float x, float y, u_int8_t * permutations);
float noise3(float x, float y, float z, u_int8_t * permutations);
float fbm2(float x, float y, int octaves, u_int8_t * permutations);
float fbm3(float x, float y, float z, int octaves, u_int8_t * permutations);
float mankind_noise2(float x, float y, u_int8_t * permutations);
float noise_layered(int count, ...);
