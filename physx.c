#include "physx.h"

void update_physics(Physics * physics, vec3_t speed)
{
	/* Add checking for max_velocity. */
	physics->velocity = v3_add(physics->velocity, speed);
	physics->velocity.x =
	  CLAMP(physics->velocity.x, -physics->max_velocity.x,
			physics->max_velocity.x);
	physics->velocity.y =
	  CLAMP(physics->velocity.y, -physics->max_velocity.y,
			physics->max_velocity.y);
	physics->velocity.z =
	  CLAMP(physics->velocity.z, -physics->max_velocity.z,
			physics->max_velocity.z);
}
