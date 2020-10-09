#pragma once

#include <stdbool.h>

#include "math_3d.h"
#include "utilities.h"
#include <stdbool.h>

typedef struct Physics {
	vec3_t velocity;
	float max_v;
	bool touches_ground;
} Physics;

void update_physics(Physics * physics, vec3_t speed);
