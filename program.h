#pragma once

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "utilities.h"

typedef struct program_t {
	GLint id;
} program_t;

program_t program_new(const char *vs_path, const char *fs_path);
void program_use();
void program_terminate();
