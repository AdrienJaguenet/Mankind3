#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
	GLuint index;
} Texture;

bool load_texture(Texture * texture, const char* path);

