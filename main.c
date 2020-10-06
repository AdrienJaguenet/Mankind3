#include <stdio.h>
#include <time.h>

#include "utilities.h"
#include "sfx.h"
#include "graphx.h"
#include "chunkmesh.h"

#define VERSION "0.0.1"

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

void handle_keystates(const Uint8 * keystates, Camera * camera,
					  unsigned int delta_ticks)
{
	if (keystates[SDL_SCANCODE_W]) {
		camera->position =
		  v3_add(camera->position,
				 v3_muls(get_Camera_lookAt(camera), 0.02 * delta_ticks));
	}

	if (keystates[SDL_SCANCODE_S]) {
		camera->position =
		  v3_sub(camera->position,
				 v3_muls(get_Camera_lookAt(camera), 0.02 * delta_ticks));
	}

	if (keystates[SDL_SCANCODE_A]) {
		camera->position =
		  v3_sub(camera->position,
				 v3_muls(get_Camera_right(camera), 0.02 * delta_ticks));
	}

	if (keystates[SDL_SCANCODE_D]) {
		camera->position =
		  v3_add(camera->position,
				 v3_muls(get_Camera_right(camera), 0.02 * delta_ticks));
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

		handle_keystates(SDL_GetKeyboardState(NULL), &gfx_context.camera,
						 delta_ticks);

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
