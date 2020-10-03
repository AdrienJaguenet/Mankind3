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

	gfx_context->camera.position = vec3(0, CHUNK_SIZE * 2, 0);
	gfx_context->camera.rotation = vec3(0, -3.1415 / 4, 0);

	init_Heap(&gfx_context->meshgen_pqueue);
}

void draw_Map(GFXContext * gfx_context, Map * map)
{
	Camera *camera = &gfx_context->camera;
	vec3_t *campos = &camera->position;
	int cx, cy, cz;
	get_chunk_pos(campos->x, campos->y, campos->z, &cx, &cy, &cz);
	for (int i = cx - 5; i < cx + 5; ++i) {
		for (int j = cy - 5; j < cy + 5; ++j) {
			for (int k = cz - 5; k < cz + 5; ++k) {
				Chunk *c = get_chunk_or_null(map, i, j, k);
				/* if the chunk does not exist */
				if (!c) {
					c = new_Chunk(map, i, j, k);
					randomly_populate(c);
					continue;
				}
				vec3_t to_chunk = v3_sub(vec3(c->x * CHUNK_SIZE * BLOCK_SIZE,
											  c->y * CHUNK_SIZE * BLOCK_SIZE,
											  c->z * CHUNK_SIZE * BLOCK_SIZE),
										 *campos);
				/* if the chunk is behind us and not the chunk we are in */
				if (v3_dot(to_chunk, get_Camera_lookAt(camera)) < 0.2f &&
					!(cx == i && cy == j && cz == k)) {
					continue;
				}
				if (!c->pending_meshgen
					&& ((!c->mesh && !c->empty) || (c->dirty))) {
					int distance = v3_length(to_chunk);
					push_Chunk_to_queue(gfx_context, c, distance);
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
	clean_Heap(&gfx_context->meshgen_pqueue);
	SDL_DestroyWindow(gfx_context->window);
	SDL_Quit();
}

void push_Chunk_to_queue(GFXContext * gfx_context, Chunk * chunk, int priority)
{
	chunk->pending_meshgen = true;
	/* The closer the chunk, the higher its priority */
	insert_HeapNode(&gfx_context->meshgen_pqueue, chunk, priority);
	gfx_context->queue_size++;
}

void gen_Chunks_in_queue(GFXContext * gfx_context, Map * map, int max_gens)
{
	int i;
	for (i = 0; i < max_gens; ++i) {
		Chunk *c = extract_HeapNode(&gfx_context->meshgen_pqueue);
		if (c == NULL) {
			break;
		}
		c->pending_meshgen = false;
		generate_chunk_mesh(c, map, &gfx_context->tilemap);
		gfx_context->queue_size--;
	}
	INFO("Generated %d meshes, %d to go", i, gfx_context->queue_size);
}
