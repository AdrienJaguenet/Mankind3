#include "events.h"

bool handle_event(SDL_Event * e, Camera * camera, Physics * physics, Map * map,
				  SFXContext * sfx_context, unsigned int delta_ticks)
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
		if (e->key.keysym.sym == SDLK_SPACE && physics->touches_ground) {
			physics->velocity.y = .2f;
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

void handle_keystates(const Uint8 * keystates, Camera * camera,
					  Physics * physics)
{
	float AXIS_VELOCITY = !physics->touches_ground ? 0.1 : 0.1;
	vec3_t new_vel =
	  !physics->touches_ground ? physics->velocity : vec3(0, 0, 0);
	if (keystates[SDL_SCANCODE_W]) {
		new_vel =
		  v3_add(new_vel, v3_muls(get_Camera_forward(camera), AXIS_VELOCITY));
	}

	if (keystates[SDL_SCANCODE_S]) {
		new_vel =
		  v3_add(new_vel, v3_muls(get_Camera_forward(camera), -AXIS_VELOCITY));
	}

	if (keystates[SDL_SCANCODE_A]) {
		new_vel =
		  v3_add(new_vel, v3_muls(get_Camera_right(camera), -AXIS_VELOCITY));
	}

	if (keystates[SDL_SCANCODE_D]) {
		new_vel =
		  v3_add(new_vel, v3_muls(get_Camera_right(camera), AXIS_VELOCITY));
	}

	if (keystates[SDL_SCANCODE_LCTRL]) {
		new_vel =
		  v3_add(new_vel, v3_muls(get_Camera_up(camera), -AXIS_VELOCITY));
	}

	new_vel = v3_muls(v3_norm(new_vel), AXIS_VELOCITY);
	physics->velocity.x = new_vel.x;
	physics->velocity.z = new_vel.z;
	//update_physics(physics, new_vel);
}
