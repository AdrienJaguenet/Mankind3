#include <stdio.h>

#include "engine.h"
#include "utilities.h"

#define VERSION "0.0.1"

int main()
{
	engine_t engine = engine_new();
	new_Chunk(&engine.map, 1, 1, 1);

	while (engine.running) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			engine_handle_event(&engine, &event);
		}

		engine_update(&engine);
		engine_render(&engine);
	}

	INFO("Mankind %s", VERSION);
	return 0;
}
