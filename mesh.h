#pragma once

#include "graphx.h"
#include "utilities.h"

typedef struct mesh_t {
	GLuint vao, vbo, nbo, uvbo, ebo;
	vec3_t *vertices, *normals;
	vec2_t *uvs;
	GLuint *indices;
} mesh_t;

mesh_t mesh_new(vec3_t * vertices, vec3_t * normals, vec2_t * uvs,
				GLuint * indices);
void mesh_render(mesh_t * mesh);
void mesh_terminate(mesh_t * mesh);
