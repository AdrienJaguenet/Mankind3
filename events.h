#pragma once
#include "soundfx.h"
#include "camera.h"
#include "physx.h"
#include "map.h"

typedef struct {
	bool moving_forward;
	bool moving_backward;
	bool jumping;
	bool running;
	bool moving_left;
	bool moving_right;
} InputState;

vec3_t get_wanted_movement_vector(InputState * state, Camera * camera);

bool handle_event(SDL_Event * e, Camera * camera, Physics * physics, Map * map,
				  SFXContext * sfx_context, unsigned int delta_ticks,
				  InputState * state);

void handle_input(InputState * state, Camera * camera, Physics * physics);
