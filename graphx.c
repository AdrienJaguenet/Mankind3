#include "graphx.h"

void init_GFX(GFXContext * gfx_context, int window_width, int window_height)
{
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
		FATAL("Could not initialize SDL: '%s'\n", SDL_GetError());
	}
	gfx_context->window = SDL_CreateWindow("Mankind 3", SDL_WINDOWPOS_UNDEFINED,
										   SDL_WINDOWPOS_UNDEFINED,
										   window_width, window_height,
										   SDL_WINDOW_OPENGL);
	if (!gfx_context->window) {
		FATAL("Failed to create SDL window: %s", SDL_GetError());
	}
	gfx_context->gl_context = SDL_GL_CreateContext(gfx_context->window);
	if (!gfx_context->gl_context) {
		FATAL("Failed to create OpenGL context: %s", SDL_GetError());
	}

	if (glewInit() != GLEW_OK) {
		FATAL("Failed to initialize GLEW");
	}

	gfx_context->main_program =
	  program_new("./resources/default.vs", "./resources/default.fs");
	load_texture(&gfx_context->tilemap, "gfx/tilemap.png");

}

void draw_Map(GFXContext * gfx_context, Map * map)
{
	(void) gfx_context;
	(void) map;
	draw_Chunk(gfx_context, get_chunk_or_null(map, 1, 1, 1));
}

void draw_Chunk(GFXContext * gfx_context, Chunk * chunk)
{
	if (chunk->mesh) {
		draw_Mesh(gfx_context, chunk->mesh, vec3(chunk->x, chunk->y, chunk->z));
	}
}

void draw_Mesh(GFXContext * gfx_context, mesh_t * mesh, vec3_t position)
{
	mat4_t model = m4_translation(position);
	glUniformMatrix4fv(glGetUniformLocation
					   (gfx_context->main_program.id, "model"), 1, GL_FALSE,
					   (float *) &model);
	glUniform3f(glGetUniformLocation
				(gfx_context->main_program.id, "ambient_light"), .5f, 1.f, .5f);
	mesh_render(mesh);
}

void begin_draw(GFXContext * gfx_context)
{
	setup_camera(&gfx_context->main_program, gfx_context->window,
				 &gfx_context->camera);
	/* clean */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Render shit here. */
	program_use(&gfx_context->main_program);

	glEnable(GL_DEPTH_TEST);
}

void end_draw(GFXContext * gfx_context)
{
	SDL_GL_SwapWindow(gfx_context->window);
}

void quit_GFX(GFXContext * gfx_context)
{
	SDL_DestroyWindow(gfx_context->window);
	SDL_Quit();
}
