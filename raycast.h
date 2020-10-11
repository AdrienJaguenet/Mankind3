#pragma once

#include <float.h>

#include "math_3d.h"
#include "map.h"

#define MAX_DISTANCE 10.0

bool raycast_block(vec3_t origin, vec3_t lookat, Map * map, vec3_t * position,
				   vec3_t * normal);
