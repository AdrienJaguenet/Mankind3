#pragma once

#include "math_3d.h"
#include "utilities.h"
#include "texture.h"
#include "position.h"

#define FACE_MASK   0x7
#define POS_MASK    0x1f
#define CORNER_MASK 0x3

#define FACE_OFFSET 0
#define POSX_OFFSET 3
#define POSY_OFFSET 8
#define POSZ_OFFSET 13
#define CORNER_OFFSET 18

#define TYPE_MASK   0x1ff
#define STRETCH_MASK 0x1f


#define STRETCHX_OFFSET 0
#define STRETCHY_OFFSET 5
#define TYPE_OFFSET 10

typedef struct mesh_t {
	GLuint vao, pbo,			/* Packed buffer object */
	 texbo,						/* Texture buffer oject */
	 ebo;
	uint32_t vertices_no;
	uint32_t vertices_max;
	GLuint *packed;
	GLuint *texinfo;
	GLuint *indices;
	Texture *texture;
} mesh_t;

void mesh_load(mesh_t * mesh);
void mesh_render(mesh_t * mesh, Texture * texture);
void mesh_terminate(mesh_t * mesh);
void mesh_push_vertex(mesh_t * mesh, ECorner corner, EFace face,
					  vec3_t position, vec2_t uv_stretch, int type);
void resize_mesh(mesh_t * mesh, int new_size);
