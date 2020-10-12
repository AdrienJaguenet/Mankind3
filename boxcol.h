#pragma once

#include "math_3d.h"
#include "map.h"

typedef struct AABB {
	vec3_t pos, dim;
} AABB;

void translate_AABB(AABB * aabb, const vec3_t vec);
AABB translated_AABB(AABB * aabb, const vec3_t vec);
bool collides(AABB * aabb1, AABB * aabb2);
bool map_collides(AABB * aabb, Map * map, vec3_t * colliding_normal);
