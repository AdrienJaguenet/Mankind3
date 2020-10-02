#pragma once

#include "math_3d.h"
#include "utilities.h"
#include "texture.h"

typedef struct mesh_t {
	GLuint vao, vbo, nbo, uvbo, ebo;
	uint32_t vertices_no;
	uint32_t vertices_max;
	vec3_t *vertices, *normals;
	vec2_t *uvs;
	GLuint *indices;
	Texture *texture;
} mesh_t;

mesh_t mesh_new(vec3_t * vertices, vec3_t * normals, vec2_t * uvs,
				GLuint * indices);
void mesh_load(mesh_t * mesh);
void mesh_render(mesh_t * mesh);
void mesh_terminate(mesh_t * mesh);
void mesh_push_vertex(mesh_t * mesh, vec3_t vertex, vec2_t uv, vec3_t normal);
void resize_mesh(mesh_t * mesh, int new_size);
