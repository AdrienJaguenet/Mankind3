#pragma once

#include "math_3d.h"
#include "map.h"

/* The position represents the middle point of the box
   The box extends dim/2 from the pos. */
typedef struct AABB {
	vec3_t pos, dim;
} AABB;

AABB translated_AABB(AABB * aabb, const vec3_t vec);
bool collides(AABB * aabb1, AABB * aabb2);
bool map_collides(AABB * aabb, Map * map, vec3_t * colliding_normal,
				  bool enabled_faces[6]);
