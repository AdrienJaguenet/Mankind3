#pragma once

#include "boxcol.h"

bool try_move(AABB * aabb, vec3_t diff, Map * map, vec3_t * collision_normal);
