#include <stdio.h>

#include "utilities.h"
#include "graphx.h"
#include "chunkmesh.h"

#define VERSION "0.0.1"

bool handle_event(SDL_Event * e)
{
	if (e->type == SDL_QUIT) {
		return false;
	} else {
		return true;
	}
}

int main()
{
	INFO("Mankind %s", VERSION);
	GFXContext gfx_context;
	init_GFX(&gfx_context, 800, 600);
	Map map;
	Chunk *c = new_Chunk(&map, 1, 1, 1);
	randomly_populate(c);
	generate_chunk_mesh(c, &map, &gfx_context.tilemap);

	bool running = true;

	while (running) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			running = handle_event(&event);
		}
		begin_draw(&gfx_context);
		draw_Chunk(&gfx_context, c);
		end_draw(&gfx_context);
	}

	quit_GFX(&gfx_context);
	INFO("Goodbye!");
	return 0;
}
