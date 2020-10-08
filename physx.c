#include "physx.h"

void update_physics(Physics * physics, vec3_t speed)
{
	physics->velocity = v3_add(physics->velocity, speed);
	float velocity_l = v3_length(physics->velocity);
	vec3_t velocity_norm = v3_norm(physics->velocity);
	velocity_l = CLAMP(velocity_l, 0., physics->max_v);
	physics->velocity = v3_muls(velocity_norm, velocity_l);
}
