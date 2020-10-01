#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

#include "math_3d.h"
#include "utilities.h"

typedef struct mesh_t {
	GLuint vao, vbo, ebo;
	vec3_t *vertices;
	GLuint *indices;
} mesh_t;

mesh_t mesh_new(vec3_t * vertices, GLuint * indices);
void mesh_render(mesh_t * mesh);
void mesh_terminate(mesh_t * mesh);
