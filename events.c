#include "events.h"
#include "raycast.h"

vec3_t get_wanted_movement_vector(InputState * state, Camera * camera)
{
	vec3_t movement = vec3(0, 0, 0);
	if (state->moving_forward) {
		movement = v3_add(movement, get_Camera_forward(camera));
	}
	if (state->moving_backward) {
		movement = v3_sub(movement, get_Camera_forward(camera));
	}
	if (state->moving_left) {
		movement = v3_sub(movement, get_Camera_right(camera));
	}
	if (state->moving_right) {
		movement = v3_add(movement, get_Camera_right(camera));
	}
	// normalize the vector
	movement = v3_norm(movement);
	return movement;
}

bool handle_event(SDL_Event * e, Camera * camera, Physics * physics, Map * map,
				  SFXContext * sfx_context, unsigned int delta_ticks,
				  InputState * state)
{
	(void) delta_ticks;
	if (e->type == SDL_QUIT) {
		return false;
	} else if (e->type == SDL_MOUSEMOTION) {
		camera->rotation.x -= e->motion.xrel / 100.f;
		camera->rotation.y -= e->motion.yrel / 100.f;
		camera->rotation.y =
		  CLAMP(camera->rotation.y, -M_PI / 2 + 0.025, M_PI / 2 - 0.025);
	} else if (e->type == SDL_KEYDOWN) {
		switch (e->key.keysym.sym) {
		  case SDLK_SPACE:
			  if (physics->touches_ground) {
				  physics->velocity.y = .2f;
				  state->jumping = true;
			  }
			  break;

		  case SDLK_w:
			  state->moving_forward = true;
			  break;

		  case SDLK_s:
			  state->moving_backward = true;
			  break;

		  case SDLK_a:
			  state->moving_left = true;
			  break;

		  case SDLK_d:
			  state->moving_right = true;
			  break;

		  case SDLK_LSHIFT:
			  state->running = true;
			  break;
		}
	} else if (e->type == SDL_KEYUP) {
		switch (e->key.keysym.sym) {
		  case SDLK_SPACE:
			  state->jumping = false;
			  break;

		  case SDLK_w:
			  state->moving_forward = false;
			  break;

		  case SDLK_s:
			  state->moving_backward = false;
			  break;

		  case SDLK_a:
			  state->moving_left = false;
			  break;

		  case SDLK_d:
			  state->moving_right = false;
			  break;

		  case SDLK_LSHIFT:
			  state->running = false;
			  break;
		}
	} else if (e->type == SDL_MOUSEBUTTONDOWN) {
		if (e->button.button == SDL_BUTTON_LEFT) {
			vec3_t normal = vec3(0, 0, 0);
			vec3_t position = vec3(0, 0, 0);
			vec3_t real_position = vec3(camera->position.x / BLOCK_SIZE,
										camera->position.y / BLOCK_SIZE,
										camera->position.z / BLOCK_SIZE);

			if (raycast_block
				(real_position, get_Camera_lookAt(camera), map, &position,
				 &normal)) {
				play_Audio_at(sfx_context, &sfx_context->effects.break_block,
							  &position);
				set_block_type(map, position.x, position.y, position.z, 0, 0);
			}
		} else if (e->button.button == SDL_BUTTON_RIGHT) {
			vec3_t normal = vec3(0, 0, 0);
			vec3_t real_position = vec3(camera->position.x / BLOCK_SIZE,
										camera->position.y / BLOCK_SIZE,
										camera->position.z / BLOCK_SIZE);
			vec3_t position = vec3(0, 0, 0);

			if (raycast_block
				(real_position, get_Camera_lookAt(camera), map, &position,
				 &normal)) {
				play_Audio_at(sfx_context, &sfx_context->effects.place_block,
							  &position);
				vec3_t temp = v3_add(position, normal);
				set_block_type(map, temp.x, temp.y, temp.z, 0, 1);
			}

		}
	}
	return true;
}

void handle_input(InputState * state, Camera * camera, Physics * physics)
{
	float AXIS_VELOCITY = 0.1;
	if (state->running) {
		AXIS_VELOCITY *= 2.0;
	}
	vec3_t new_vel =
	  !physics->touches_ground ? physics->
	  velocity : get_wanted_movement_vector(state,
											camera);

	new_vel = v3_muls(v3_norm(new_vel), AXIS_VELOCITY);
	physics->velocity.x = new_vel.x;
	physics->velocity.z = new_vel.z;
	//update_physics(physics, new_vel);
}
