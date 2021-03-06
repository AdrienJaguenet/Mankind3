#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "utilities.h"

typedef enum {
	UV_TOP_LEFT = 0,
	UV_TOP_RIGHT = 1,
	UV_BOTTOM_LEFT = 2,
	UV_BOTTOM_RIGHT = 3
} UVCorner;

typedef struct {
	GLuint index;
	int size;					// All texture are assumed to be be square
} Texture;

bool load_texture(Texture * texture, const char *path);

void get_tex_quad(Texture * texture, int index, int quad_size,
				  vec2_t corners[4]);
