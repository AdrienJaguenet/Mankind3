#pragma once

#include "math_3d.h"
#include "utilities.h"

typedef struct Physics {
	vec3_t velocity;
	vec3_t max_velocity;
} Physics;

void update_physics(Physics * physics, vec3_t speed);
