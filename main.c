#include <stdio.h>
#include <time.h>

#include "utilities.h"
#include "sfx.h"
#include "graphx.h"
#include "chunkmesh.h"
#include "boxcol.h"
#include "physx.h"

#define VERSION "0.0.1"

void try_move(AABB * aabb, Physics * physics, Map * map)
{
	/* Calculate the length only once */
	float diff_l = v3_length(physics->velocity);

	do {
		AABB translated = translated_AABB(aabb, physics->velocity);
		if (!map_collides(&translated, map)) {
			*aabb = translated;
			break;
		} else {
			/* try again by moving only halfway */
			physics->velocity = v3_divs(physics->velocity, 2.f);
			diff_l /= 2.f;
		}
	} while (diff_l > 0.05);

	if (diff_l <= 0.05) {
		physics->velocity = vec3(0, 0, 0);
		physics->velocity.y = 0;
	}
}

bool handle_event(SDL_Event * e, Camera * camera, Physics * physics,
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
	} else if (e->type == SDL_KEYUP) {
		physics->velocity.x = 0;
		physics->velocity.z = 0;
	}
	return true;
}

void handle_keystates(const Uint8 * keystates, AABB * player, Camera * camera,
					  Physics * physics, Map * map, unsigned int delta_ticks)
{
	/* This shit moves relative to the camera look at, instead of some
	   front or sum shit. So it be broken kinda. */
	if (keystates[SDL_SCANCODE_W]) {
		update_physics(physics,
					   v3_muls(get_Camera_forward(camera), 0.02 * delta_ticks));
		try_move(player, physics, map);
	}

	if (keystates[SDL_SCANCODE_S]) {
		update_physics(physics,
					   v3_muls(get_Camera_forward(camera),
							   -0.02 * delta_ticks));
	}

	if (keystates[SDL_SCANCODE_A]) {
		update_physics(physics,
					   v3_muls(get_Camera_right(camera), -0.02 * delta_ticks));
	}

	if (keystates[SDL_SCANCODE_D]) {
		update_physics(physics,
					   v3_muls(get_Camera_right(camera), 0.02 * delta_ticks));
	}

	/* This should be an SDL event (not a keystate) in the future, though. */
	if (keystates[SDL_SCANCODE_SPACE]) {
		physics->velocity = vec3(0.0, 0.05 * delta_ticks, 0.0);
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
	init_SFX();
	Map *map = calloc(sizeof(Map), 1);
	map->hash = shuffled_hash();

	map->chunks = calloc(sizeof(Chunk *), MAX_CHUNKS_NO);

	AABB player = { vec3(0, 10, 0), vec3(0.95, 2., 0.95) };
	Physics physics = { vec3(0, 0, 0), vec3(0.1, 0.4, 0.1) };

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
			  handle_event(&event, &gfx_context.camera, &physics, delta_ticks);
		}

		handle_keystates(SDL_GetKeyboardState(NULL), &player,
						 &gfx_context.camera, &physics, map, delta_ticks);

		update_physics(&physics, vec3(0.0, -0.01 * delta_ticks, 0.0));
		try_move(&player, &physics, map);
		attach_camera_to(&gfx_context.camera, &player);
		gen_ChunkMesh_in_queue(&gfx_context, map, 10);
		gen_Chunks_in_queue(&gfx_context, map, 10);
		begin_draw(&gfx_context);
		draw_Map(&gfx_context, map);
		end_draw(&gfx_context);
	}

	quit_GFX(&gfx_context);
	delete_Map(map);
	free(map->chunks);
	free(map);
	INFO("Goodbye!");
	return 0;
}
