#pragma once

#include <math.h>
#include <stdlib.h>
#include <stdarg.h>

#include "utilities.h"

char *shuffled_permutations(size_t size);
float noise2(float x, float y, char *permutations);
float noise3(float x, float y, float z, char *permutations);
float noise_layered(int count, ...);
float fractal2(float x, float y, char *permutations);
float fractal3(float x, float y, float z, char *permutations);
