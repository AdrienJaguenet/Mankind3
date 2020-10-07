#include <stdio.h>
#include <time.h>

#include "utilities.h"
#include "sfx.h"
#include "graphx.h"
#include "chunkmesh.h"
#include "boxcol.h"

#define VERSION "0.0.1"

void try_move(AABB * aabb, vec3_t where, Camera * camera, Map * map)
{
	AABB translated = translated_AABB(aabb, where);

	if (!map_collides(&translated, map)) {
		translate_AABB(aabb, where);
		camera->position =
		  vec3((aabb->min.x + aabb->max.x) / 2, aabb->min.y - 5,
			   (aabb->min.z + aabb->max.z) / 2);
	}
}

bool handle_event(SDL_Event * e, Camera * camera, unsigned int delta_ticks)
{
	(void) delta_ticks;
	if (e->type == SDL_QUIT) {
		return false;
	} else if (e->type == SDL_MOUSEMOTION) {
		camera->rotation.x -= e->motion.xrel / 100.f;
		camera->rotation.y -= e->motion.yrel / 100.f;
		camera->rotation.y =
		  CLAMP(camera->rotation.y, -M_PI / 2 + 0.025, M_PI / 2 - 0.025);
	}
	return true;
}

void handle_keystates(const Uint8 * keystates, AABB * player, Camera * camera,
					  Map * map, unsigned int delta_ticks)
{
	/* This shit moves relative to the camera look at, instead of some
	   front or sum shit. So it be broken kinda. */
	if (keystates[SDL_SCANCODE_W]) {
		try_move(player, v3_muls(get_Camera_lookAt(camera), 0.02 * delta_ticks),
				 camera, map);
	}

	if (keystates[SDL_SCANCODE_S]) {
		try_move(player,
				 v3_muls(get_Camera_lookAt(camera), -0.02 * delta_ticks),
				 camera, map);
	}

	if (keystates[SDL_SCANCODE_A]) {
		try_move(player, v3_muls(get_Camera_right(camera), -0.02 * delta_ticks),
				 camera, map);
	}

	if (keystates[SDL_SCANCODE_D]) {
		try_move(player, v3_muls(get_Camera_right(camera), 0.02 * delta_ticks),
				 camera, map);
	}

	/* This should be an SDL event (not a keystate) in the future, though. */
	if (keystates[SDL_SCANCODE_SPACE]) {
		try_move(player, vec3(0.0, 0.05 * delta_ticks, 0.0), camera, map);
	}
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

	AABB player =
	  { vec3(0, CHUNK_SIZE * 2, 0), vec3(0.95, 1.6 * CHUNK_SIZE * 2, 0.95) };

	bool running = true;
	unsigned int delta_ticks;
	unsigned int last_ticks = SDL_GetTicks();

	while (running) {
		SDL_Event event;

		unsigned int new_ticks = SDL_GetTicks();
		delta_ticks = new_ticks - last_ticks;
		last_ticks = new_ticks;

		while (SDL_PollEvent(&event)) {
			running = handle_event(&event, &gfx_context.camera, delta_ticks);
		}

		handle_keystates(SDL_GetKeyboardState(NULL), &player,
						 &gfx_context.camera, map, delta_ticks);

		try_move(&player, vec3(0.0, -0.02 * delta_ticks, 0.0),
				 &gfx_context.camera, map);
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
