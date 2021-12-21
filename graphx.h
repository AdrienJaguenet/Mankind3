#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>

#include "math_3d.h"
#include "program.h"

#include "map.h"
#include "texture.h"
#include "camera.h"
#include "heap.h"
#include "ui.h"
#include "cubehighlight.h"

typedef struct {
	SDL_Window *window;
	SDL_GLContext gl_context;

	program_t chunk_program;
	Texture tilemap;
	Camera camera;
	int queue_size;
	Heap render_pqueue;
	Heap meshgen_pqueue;
	Heap terrgen_pqueue;
	UI ui;
	CubeHighlight cube_highlight;
} GFXContext;

void init_GFX(GFXContext * gfx_context, int window_width, int window_height);

void init_UI(GFXContext * gfx_context);

void draw_UI(GFXContext * gfx_context);

void draw_Map(GFXContext * gfx_context, Map * map);

void draw_Chunks(GFXContext * gfx_context, int max_gens);

void draw_Mesh(GFXContext * gfx_context, mesh_t * mesh, vec3_t position,
			   int lod);

void draw_CubeHighlight(GFXContext * gfx_context, Map * map);

void quit_GFX(GFXContext * gfx_context);

void begin_draw(GFXContext * gfx_context);

void end_draw(GFXContext * gfx_context);

void push_Chunk_to_queue(GFXContext * gfx_context, Chunk * chunk, int priority,
						 ChunkPending pending, Heap * queue);

void gen_ChunkMesh_in_queue(GFXContext * gfx_context, Map * map, int max_gens);
void gen_Chunks_in_queue(GFXContext * gfx_context, Map * map, int max_gens);

void dbg_gl();
