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

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	gfx_context->gl_context = SDL_GL_CreateContext(gfx_context->window);

	INFO("Graphic card vendor: '%s'", glGetString(GL_VENDOR));
	INFO("Renderer: '%s'", glGetString(GL_RENDERER));
	INFO("GL version: '%s'", glGetString(GL_VERSION));
	INFO("GLSL version: '%s'", glGetString(GL_SHADING_LANGUAGE_VERSION));

	if (!gfx_context->gl_context) {
		FATAL("Failed to create OpenGL context: %s", SDL_GetError());
	}

	int error = glewInit();
	if (glewInit() != GLEW_OK) {
		FATAL("Failed to initialize GLEW: %s", glewGetErrorString(error));
	}

	glClearColor(0.53, 0.81, 0.92, 1.0);

	gfx_context->main_program =
	  program_new("./resources/shaders/default.vs",
				  "./resources/shaders/default.fs");
	load_texture(&gfx_context->tilemap, "./resources/gfx/tilemap.png");

	gfx_context->camera.position = vec3(0, CHUNK_SIZE * 2, 0);
	gfx_context->camera.rotation = vec3(0, -3.1415 / 4, 0);

	init_Heap(&gfx_context->meshgen_pqueue);
	init_Heap(&gfx_context->terrgen_pqueue);
}

bool cull_chunk(int x, int y, int z, Camera * camera, float *distance)
{
	vec3_t *campos = &camera->position;
	float factor = CHUNK_SIZE * BLOCK_SIZE;
	vec3_t chunkpos = vec3(x * factor, y * factor, z * factor);
	vec3_t to_corner = v3_sub(chunkpos, *campos);
	*distance = v3_length(to_corner);
	if (*distance >= 1024.f) {
		return true;
	}
	/* Look for any corner that we should be seeing */
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 2; ++j) {
			for (int k = 0; k < 2; ++k) {
				vec3_t corner =
				  v3_add(vec3(i * factor, j * factor, k * factor), chunkpos);
				to_corner = v3_norm(v3_sub(corner, *campos));
				if (v3_dot(to_corner, get_Camera_lookAt(camera)) >= 0.f) {
					return false;
				}
			}
		}
	}
	return true;
}

void draw_Map(GFXContext * gfx_context, Map * map)
{
	Camera *camera = &gfx_context->camera;
	vec3_t *campos = &camera->position;
	int cx, cy, cz;
	const int RENDER_DISTANCE = 4;
	get_chunk_pos(campos->x, campos->y, campos->z, &cx, &cy, &cz);
	for (int i = cx - RENDER_DISTANCE; i < cx + RENDER_DISTANCE; ++i) {
		for (int j = cy - RENDER_DISTANCE; j < cy + RENDER_DISTANCE; ++j) {
			for (int k = cz - RENDER_DISTANCE; k < cz + RENDER_DISTANCE; ++k) {
				Chunk *c = get_chunk_or_null(map, i, j, k);
				float distance;
				if (cull_chunk(i, j, k, camera, &distance)) {
					continue;
				}
				/* if the chunk does not exist */
				if (!c) {
					c = new_Chunk(map, i, j, k);
					push_Chunk_to_queue(gfx_context, c, distance,
										PENDING_TERRAIN,
										&gfx_context->terrgen_pqueue);
					continue;
				}
				if (!c->pending[PENDING_TERRAIN] && !c->pending[PENDING_MESHGEN]
					&& ((!c->mesh[0] && !c->empty) || (c->dirty))) {
					push_Chunk_to_queue(gfx_context, c, distance,
										PENDING_MESHGEN,
										&gfx_context->meshgen_pqueue);
				}
				int lod = 0;
				draw_Chunk(c, gfx_context, lod);
			}
		}
	}
}

void draw_Chunk(Chunk * chunk, GFXContext * gfx_context, int lod)
{
	if (chunk->mesh[lod]) {
		draw_Mesh(gfx_context, chunk->mesh[lod],
				  vec3(chunk->x * CHUNK_SIZE * BLOCK_SIZE,
					   chunk->y * CHUNK_SIZE * BLOCK_SIZE,
					   chunk->z * CHUNK_SIZE * BLOCK_SIZE), lod);
	}
}

void draw_Mesh(GFXContext * gfx_context, mesh_t * mesh, vec3_t position,
			   int lod)
{
	mat4_t model = m4_translation(position);
	glUniformMatrix4fv(glGetUniformLocation
					   (gfx_context->main_program.id, "model"), 1, GL_FALSE,
					   (float *) &model);
	glUniform3f(glGetUniformLocation
				(gfx_context->main_program.id, "ambient_light"), .5f, 1.f, .5f);
	glUniform1i(glGetUniformLocation
				(gfx_context->main_program.id, "tilemap_grid_size"), 8);
	glUniform1i(glGetUniformLocation(gfx_context->main_program.id, "lod"), lod);
	mesh_render(mesh, &gfx_context->tilemap);
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
	SDL_GL_DeleteContext(gfx_context->gl_context);
	SDL_DestroyWindow(gfx_context->window);
	SDL_Quit();
}

void push_Chunk_to_queue(GFXContext * gfx_context, Chunk * chunk, int priority,
						 ChunkPending pending, Heap * queue)
{
	chunk->pending[pending] = true;
	/* The closer the chunk, the higher its priority */
	insert_HeapNode(queue, chunk, priority);
	gfx_context->queue_size++;
}

void gen_ChunkMesh_in_queue(GFXContext * gfx_context, Map * map, int max_gens)
{
	int i;
	for (i = 0; i < max_gens; ++i) {
		Chunk *c = extract_HeapNode(&gfx_context->meshgen_pqueue);
		if (c == NULL) {
			break;
		}
		c->pending[PENDING_MESHGEN] = false;
		generate_chunk_mesh(c, map);
		gfx_context->queue_size--;
	}
}

void gen_Chunks_in_queue(GFXContext * gfx_context, Map * map, int max_gens)
{
	int i;
	for (i = 0; i < max_gens; ++i) {
		Chunk *c = extract_HeapNode(&gfx_context->terrgen_pqueue);
		if (c == NULL) {
			break;
		}
		c->pending[PENDING_TERRAIN] = false;
		randomly_populate(map, c);
		gen_Chunk_LOD(c);
		gfx_context->queue_size--;
	}

}
