#include <stdio.h>
#include <time.h>

#include "utilities.h"
#include "sfx.h"
#include "graphx.h"
#include "chunkmesh.h"
#include "boxcol.h"
#include "physx.h"
#include "raycast.h"

#define VERSION "0.0.1"

bool try_move(AABB * aabb, vec3_t diff, Map * map)
{
	/* Calculate the length only once */
	float diff_l = v3_length(diff);

	do {
		AABB translated = translated_AABB(aabb, diff);
		if (!map_collides(&translated, map)) {
			*aabb = translated;
			return true;
		} else {
			/* try again by moving only halfway */
			diff = v3_divs(diff, 2.f);
			diff_l /= 2.f;
		}
	} while (diff_l > 0.05);
	return false;
}

bool handle_event(SDL_Event * e, Camera * camera, Physics * physics, Map * map,
				  unsigned int delta_ticks)
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
			physics->velocity.y = 0.015f * delta_ticks;
		}
	} else if (e->type == SDL_MOUSEBUTTONDOWN) {
		if (e->button.button == SDL_BUTTON_LEFT) {
			vec3_t normal = vec3(0, 0, 0);
			vec3_t position = vec3(0, 0, 0);

			if (raycast_block
				(camera->position, get_Camera_lookAt(camera), map, &position,
				 &normal)) {
				set_block_type(map, position.x, position.y, position.z, 0, 0);
			}
		} else if (e->button.button == SDL_BUTTON_RIGHT) {
			vec3_t normal = vec3(0, 0, 0);
			vec3_t position = vec3(0, 0, 0);

			if (raycast_block
				(camera->position, get_Camera_lookAt(camera), map, &position,
				 &normal)) {
				vec3_t temp = v3_add(position, normal);
				set_block_type(map, temp.x, temp.y, temp.z, 0, 1);
			}

		}
	}
	return true;
}

void handle_keystates(const Uint8 * keystates, AABB * player, Camera * camera,
					  Physics * physics, Map * map, unsigned int delta_ticks)
{
	(void) physics;
	/* This shit moves relative to the camera look at, instead of some
	   front or sum shit. So it be broken kinda. */
	float dt_factor = delta_ticks * 0.008;
	if (keystates[SDL_SCANCODE_W]) {
		try_move(player, v3_muls(get_Camera_forward(camera), dt_factor), map);
	}

	if (keystates[SDL_SCANCODE_S]) {
		try_move(player, v3_muls(get_Camera_forward(camera), -dt_factor), map);
	}

	if (keystates[SDL_SCANCODE_A]) {
		try_move(player, v3_muls(get_Camera_right(camera), -dt_factor), map);
	}

	if (keystates[SDL_SCANCODE_D]) {
		try_move(player, v3_muls(get_Camera_right(camera), dt_factor), map);
	}

	if (keystates[SDL_SCANCODE_LCTRL]) {
		try_move(player, v3_muls(get_Camera_up(camera), -dt_factor), map);
	}
}

void attach_camera_to(Camera * camera, AABB * aabb)
{
	camera->position =
	  vec3((aabb->pos.x + aabb->dim.x / 2),
		   (aabb->pos.y + aabb->dim.y), (aabb->pos.z + aabb->dim.z / 2));
}

int main()
{
	INFO("Mankind %s", VERSION);
	GFXContext gfx_context;
	SFXContext sfx_context;
	(void) sfx_context;
	init_GFX(&gfx_context, 1024, 768);
	SDL_InitSubSystem(SDL_INIT_AUDIO);
	init_SFX();
	load_SFX(&sfx_context);
	Map *map = calloc(sizeof(Map), 1);
	map->permutations = shuffled_permutations(512);

	for (int i = 0; i < MAX_LOD; ++i) {
		map->chunks[i] = calloc(sizeof(Chunk *), MAX_CHUNKS_NO);
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
						   delta_ticks);
		}

		handle_keystates(SDL_GetKeyboardState(NULL), &player,
						 &gfx_context.camera, &physics, map, delta_ticks);

		update_physics(&physics, vec3(0.0, -0.0005 * delta_ticks, 0.0));
		if (!try_move(&player, physics.velocity, map)
			&& physics.velocity.y < 0.f) {
			physics.velocity.y = 0;
			physics.touches_ground = true;
		} else {
			physics.touches_ground = false;
		}
		attach_camera_to(&gfx_context.camera, &player);
		gen_ChunkMesh_in_queue(&gfx_context, map, 10);
		gen_Chunks_in_queue(&gfx_context, map, 10);
		begin_draw(&gfx_context);
		draw_Map(&gfx_context, map);
		end_draw(&gfx_context);
	}

	quit_GFX(&gfx_context);
	clean_SFX(&sfx_context);
	delete_Map(map);
	free(map);
	INFO("Goodbye!");
	return 0;
}
