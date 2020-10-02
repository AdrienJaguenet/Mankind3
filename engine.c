#include "engine.h"
#include "chunkmesh.h"
#include "camera.h"
#include "math_3d.h"

engine_t *engine_new()
{
	engine_t *engine = calloc(sizeof(engine_t), 1);
	engine->map.chunks_no = 0;
	engine->map.root = NULL;

	engine->window = SDL_CreateWindow("Mankind 3", SDL_WINDOWPOS_UNDEFINED,
									  SDL_WINDOWPOS_UNDEFINED, 800, 600,
									  SDL_WINDOW_OPENGL);

	if (!engine->window) {
		FATAL("Failed to create SDL window: %s", SDL_GetError());
	}

	engine->context = SDL_GL_CreateContext(engine->window);

	if (!engine->context) {
		FATAL("Failed to create OpenGL context: %s", SDL_GetError());
	}

	if (glewInit() != GLEW_OK) {
		FATAL("Failed to initialize GLEW");
	}

	engine->program =
	  program_new("./resources/default.vs", "./resources/default.fs");
	load_texture(&engine->tilemap, "gfx/tilemap.png");

	Chunk *chunk = new_Chunk(&engine->map, 0, 0, 0);
	randomly_populate(chunk);
	generate_chunk_mesh(chunk, &engine->map, &engine->tilemap);
	engine->mesh = chunk->mesh;


	engine->last_time = SDL_GetPerformanceCounter();
	engine->running = true;
	return engine;
}

void engine_handle_event(engine_t * engine, SDL_Event * event)
{
	switch (event->type) {
	  case SDL_QUIT:
		  engine->running = false;
	}
}

void engine_update(engine_t * engine)
{
	unsigned int current_time = SDL_GetPerformanceCounter();
	engine->delta_time =
	  (float) ((current_time -
				engine->last_time) * 1000 /
			   (double) SDL_GetPerformanceFrequency());
	engine->last_time = current_time;
}

void engine_render(engine_t * engine, Camera * camera)
{
	setup_camera(&engine->program, engine->window, camera);

	/* clean */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Render shit here. */
	program_use(&engine->program);
	/* The ONE chunk at 1,1,1 */
	mat4_t model = m4_translation(vec3(1,1,1));
	glUniformMatrix4fv(glGetUniformLocation(engine->program.id, "model"), 1, GL_FALSE, (float*) &model);
	mesh_render(engine->mesh);

	SDL_GL_SwapWindow(engine->window);
}

void engine_terminate(engine_t * engine)
{
	mesh_terminate(engine->mesh);
	SDL_DestroyWindow(engine->window);
}
