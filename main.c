#include <stdio.h>
#include <time.h>

#include "utilities.h"
#include "graphx.h"
#include "soundfx.h"
#include "chunkmesh.h"
#include "boxcol.h"
#include "physx.h"
#include "raycast.h"

#define VERSION "0.0.1"

bool try_move(AABB * aabb, vec3_t diff, Map * map, vec3_t * collision_normal)
{
	/* Calculate the length only once */
	float diff_l = v3_length(diff);

	do {
		AABB translated = translated_AABB(aabb, diff);
		if (!map_collides(&translated, map, collision_normal)) {
			*aabb = translated;
			return true;
		} else {
			/* try again by moving only halfway */
			diff = v3_divs(diff, 2.f);
			diff_l /= 2.f;
		}
	} while (diff_l > 0.01);
	return false;
}

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

			if (raycast_block
				(camera->position, get_Camera_lookAt(camera), map, &position,
				 &normal)) {
				play_Audio_at(sfx_context, &sfx_context->effects.break_block,
							  &position);
				set_block_type(map, position.x, position.y, position.z, 0, 0);
			}
		} else if (e->button.button == SDL_BUTTON_RIGHT) {
			vec3_t normal = vec3(0, 0, 0);
			vec3_t position = vec3(0, 0, 0);

			if (raycast_block
				(camera->position, get_Camera_lookAt(camera), map, &position,
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
	float AXIS_VELOCITY = !physics->touches_ground ? 0.001 : 0.1;
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

	physics->velocity.x = new_vel.x;
	physics->velocity.z = new_vel.z;
	//update_physics(physics, new_vel);
}

void attach_camera_to(Camera * camera, AABB * aabb)
{
	camera->position =
	  vec3((aabb->pos.x + aabb->dim.x / 2),
		   (aabb->pos.y + 0.75 * aabb->dim.y), (aabb->pos.z + aabb->dim.z / 2));
}

int main()
{
	INFO("Mankind %s", VERSION);
	GFXContext gfx_context;
	SFXContext sfx_context;
	init_GFX(&gfx_context, 1024, 768);
	init_SFX(&sfx_context);
	play_Audio(&sfx_context, &sfx_context.music.ambient, true, 0.0);	/* Muted... LOL */
	Map *map = calloc(sizeof(Map), 1);
	srand(SEED);
	map->height_perm = shuffled_permutations(256);
	map->wet_perm = shuffled_permutations(256);
	map->temp_perm = shuffled_permutations(256);

	for (int i = 0; i < MAX_LOD; ++i) {
		init_HashMap(&map->chunks[i]);
	}

	AABB player = { vec3(0, 10, 0), vec3(0.7, 2., 0.7) };
	Physics physics = { vec3(0, 0, 0), 0.5, false };

	bool running = true;
	unsigned int delta_ticks;
	unsigned int last_ticks = SDL_GetTicks();

	while (running) {
		SDL_Event event;

		unsigned int new_ticks = SDL_GetTicks();
		delta_ticks = new_ticks - last_ticks;
		last_ticks = new_ticks;

		while (SDL_PollEvent(&event)) {
			running =
			  handle_event(&event, &gfx_context.camera, &physics, map,
						   &sfx_context, delta_ticks);
		}

		handle_keystates(SDL_GetKeyboardState(NULL),
						 &gfx_context.camera, &physics);

		vec3_t collision_normal = vec3(0, 0, 0);
		update_physics(&physics, vec3(0.0, -0.0005 * delta_ticks, 0.0));
		if (!try_move(&player, physics.velocity, map, &collision_normal)) {
			if (collision_normal.x != 0) {
				physics.velocity.x = 0;
				try_move(&player, physics.velocity, map, &collision_normal);
				if (collision_normal.z != 0) {
					physics.velocity.z = 0;
					try_move(&player, physics.velocity, map, &collision_normal);
				} else if (collision_normal.y != 0) {
					physics.velocity.y = 0;
					try_move(&player, physics.velocity, map, &collision_normal);
				}
			}
			if (collision_normal.z != 0) {
				physics.velocity.z = 0;
				try_move(&player, physics.velocity, map, &collision_normal);
				if (collision_normal.x != 0) {
					physics.velocity.x = 0;
					try_move(&player, physics.velocity, map, &collision_normal);
				} else if (collision_normal.y != 0) {
					physics.velocity.y = 0;
					try_move(&player, physics.velocity, map, &collision_normal);
				}
			}
			if (collision_normal.y != 0) {
				if (!physics.touches_ground) {
					physics.touches_ground = true;
				}
				physics.velocity.y = 0;
				try_move(&player, physics.velocity, map, &collision_normal);
				if (collision_normal.x != 0) {
					physics.velocity.x = 0;
					try_move(&player, physics.velocity, map, &collision_normal);
				} else if (collision_normal.z != 0) {
					physics.velocity.z = 0;
					try_move(&player, physics.velocity, map, &collision_normal);
				}
			}
		} else {
			if (physics.velocity.y != 0) {
				physics.touches_ground = false;
			}
		}
		/* This doesn't have to be done every frame, tbh… */
		update_al_listener(player.pos, get_Camera_lookAt(&gfx_context.camera),
						   get_Camera_up(&gfx_context.camera));

		attach_camera_to(&gfx_context.camera, &player);
		gen_ChunkMesh_in_queue(&gfx_context, map, 10);
		gen_Chunks_in_queue(&gfx_context, map, 10);
		begin_draw(&gfx_context);
		draw_Map(&gfx_context, map);
		end_draw(&gfx_context);
	}

	quit_GFX(&gfx_context);
	quit_SFX(&sfx_context);
	delete_Map(map);
	free(map);
	INFO("Goodbye!");
	return 0;
}
