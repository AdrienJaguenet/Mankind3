#include "graphx.h"

#include "chunkmesh.h"

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

	SDL_SetRelativeMouseMode(SDL_TRUE);

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

	gfx_context->first_gen_chunk = NULL;
	gfx_context->last_gen_chunk = NULL;
	gfx_context->queue_size = 0;
}

void draw_Map(GFXContext * gfx_context, Map * map)
{
  Camera *camera = &gfx_context->camera;
  vec3_t *campos = &camera->position;
  int cx, cy, cz;
  get_chunk_pos(campos->x, campos->y, campos->z, &cx, &cy, &cz);
  for (int i = cx - 3; i < cx + 3; ++i) {
	for (int j = cy - 3; j < cy + 3; ++j) {
	  for (int k = cz - 3; k < cz + 3; ++k) {
		Chunk *c = get_chunk_or_null(map, i, j, k);
		if (! c){
		  c = new_Chunk(map, i, j, k);
		  randomly_populate(c);
		}
		if ((! c->mesh && !  c->empty) || (c->dirty && ! c->pending_meshgen)) {
		  if (! c->mesh) {
		  }
		  push_Chunk_to_queue(gfx_context, c);
		}
		draw_Chunk(c, gfx_context);
	  }
	}
  }
}

void draw_Chunk(Chunk * chunk, void *gfx_context_ptr)
{
	GFXContext *gfx_context = (GFXContext *) gfx_context_ptr;
	if (chunk->mesh) {
		draw_Mesh(gfx_context, chunk->mesh,
				  vec3(chunk->x * CHUNK_SIZE * BLOCK_SIZE,
					   chunk->y * CHUNK_SIZE * BLOCK_SIZE,
					   chunk->z * CHUNK_SIZE * BLOCK_SIZE));
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

void push_Chunk_to_queue(GFXContext* gfx_context, Chunk* chunk)
{
  ChunkGenQueue* elm = malloc(sizeof(ChunkGenQueue));
  chunk->pending_meshgen = true;
  elm->next = gfx_context->first_gen_chunk;
  elm->chunk = chunk;
  if (gfx_context->last_gen_chunk == NULL) {
	gfx_context->last_gen_chunk = elm;
  }
  gfx_context->first_gen_chunk = elm;
  gfx_context->queue_size++;
}

void gen_Chunks_in_queue(GFXContext* gfx_context, Map* map, int max_gens)
{
  for (int i = 0; i < max_gens; ++i) {
	ChunkGenQueue* elm = gfx_context->first_gen_chunk;
	if (! elm) {
	  break;
	}
	generate_chunk_mesh(elm->chunk, map, &gfx_context->tilemap);
	elm->chunk->pending_meshgen = false;
	gfx_context->first_gen_chunk = elm->next;
	free(elm);
	gfx_context->queue_size--;
  }
}

