#include <stdio.h>
#include <time.h>

#include "utilities.h"
#include "graphx.h"
#include "chunkmesh.h"

#define VERSION "0.0.1"

bool handle_event(SDL_Event * e, Camera * camera)
{
	if (e->type == SDL_QUIT) {
		return false;
	} else if (e->type == SDL_KEYDOWN) {
		switch (e->key.keysym.sym) {
		  case SDLK_DOWN:
			  camera->position.z -= .5f;
			  break;
		  case SDLK_UP:
			  camera->position.z += .5f;
			  break;
		  case SDLK_LEFT:
			  camera->position.x += .5f;
			  break;
		  case SDLK_RIGHT:
			  camera->position.x -= .5f;
			  break;
		  case SDLK_SPACE:
			  camera->position.y += .5f;
			  break;
		  case SDLK_LCTRL:
			  camera->position.y -= .5f;
			  break;
		}
	} else if (e->type == SDL_MOUSEMOTION) {
		camera->rotation.x -= e->motion.xrel / 100.f;
		camera->rotation.y -= e->motion.yrel / 100.f;
	}
	return true;
}

int main()
{
	srand(time(NULL));
	INFO("Mankind %s", VERSION);
	GFXContext gfx_context;
	init_GFX(&gfx_context, 800, 600);
	Map map = { 0 };
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 1; ++j) {
			for (int k = 0; k < 4; ++k) {
				Chunk *c = new_Chunk(&map, i, j, k);
				randomly_populate(c);
				generate_chunk_mesh(c, &map, &gfx_context.tilemap);
			}
		}
	}

	bool running = true;

	while (running) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			running = handle_event(&event, &gfx_context.camera);
		}
		begin_draw(&gfx_context);
		draw_Map(&gfx_context, &map);
		end_draw(&gfx_context);
	}

	quit_GFX(&gfx_context);
	INFO("Goodbye!");
	return 0;
}
