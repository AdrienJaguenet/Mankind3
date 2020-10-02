#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>

#include "math_3d.h"
#include "program.h"

#include "map.h"
#include "texture.h"
#include "camera.h"

typedef struct {
	SDL_Window *window;
	SDL_GLContext gl_context;

	program_t main_program;
	Texture tilemap;
	Camera camera;
} GFXContext;

void init_GFX(GFXContext * gfx_context, int window_width, int window_height);

void draw_Map(GFXContext * gfx_context, Map * map);

void draw_Chunk(Chunk * chunk, void *gfx_context);

void draw_Mesh(GFXContext * gfx_context, mesh_t * mesh, vec3_t position);

void quit_GFX(GFXContext * gfx_context);

void begin_draw(GFXContext * gfx_context);

void end_draw(GFXContext * gfx_context);
