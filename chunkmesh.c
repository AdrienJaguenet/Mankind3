#include "chunkmesh.h"
#include <stdlib.h>

/*
	Y  Z
	^ /
	|/
	0--> X

	  3 -- 7
	 /|   /|
	2 +- 6 |
	| |  | |
	| 1 -+ 5
	|/   |/
	0 -- 4
 */

void push_face(mesh_t * mesh, int x, int y, int z, EFace face, int type)
{
	vec3_t vertices[8] = {
		vec3(x, y, z),
		vec3(x, y, z + 1),
		vec3(x, y + 1, z),
		vec3(x, y + 1, z + 1),
		vec3(x + 1, y, z),
		vec3(x + 1, y, z + 1),
		vec3(x + 1, y + 1, z),
		vec3(x + 1, y + 1, z + 1),
	};
	int indices[4] = { 0 };
	switch (face) {
	  case FACE_LEFT:
		  indices[CORNER_TOP_LEFT] = 3;
		  indices[CORNER_TOP_RIGHT] = 2;
		  indices[CORNER_BOTTOM_LEFT] = 1;
		  indices[CORNER_BOTTOM_RIGHT] = 0;
		  break;

	  case FACE_RIGHT:
		  indices[CORNER_TOP_LEFT] = 6;
		  indices[CORNER_TOP_RIGHT] = 7;
		  indices[CORNER_BOTTOM_LEFT] = 4;
		  indices[CORNER_BOTTOM_RIGHT] = 5;
		  break;

	  case FACE_DOWN:
		  indices[CORNER_TOP_LEFT] = 0;
		  indices[CORNER_TOP_RIGHT] = 4;
		  indices[CORNER_BOTTOM_LEFT] = 1;
		  indices[CORNER_BOTTOM_RIGHT] = 5;
		  break;

	  case FACE_UP:
		  indices[CORNER_TOP_LEFT] = 3;
		  indices[CORNER_TOP_RIGHT] = 7;
		  indices[CORNER_BOTTOM_LEFT] = 2;
		  indices[CORNER_BOTTOM_RIGHT] = 6;
		  break;

	  case FACE_BACK:
		  indices[CORNER_TOP_LEFT] = 7;
		  indices[CORNER_TOP_RIGHT] = 3;
		  indices[CORNER_BOTTOM_LEFT] = 5;
		  indices[CORNER_BOTTOM_RIGHT] = 1;
		  break;

	  case FACE_FRONT:
		  indices[CORNER_TOP_LEFT] = 2;
		  indices[CORNER_TOP_RIGHT] = 6;
		  indices[CORNER_BOTTOM_LEFT] = 0;
		  indices[CORNER_BOTTOM_RIGHT] = 4;
		  break;
	}
	mesh_push_vertex(mesh, CORNER_TOP_LEFT, face,
					 vertices[indices[CORNER_TOP_LEFT]], type);
	mesh_push_vertex(mesh, CORNER_TOP_RIGHT, face,
					 vertices[indices[CORNER_TOP_RIGHT]], type);
	mesh_push_vertex(mesh, CORNER_BOTTOM_LEFT, face,
					 vertices[indices[CORNER_BOTTOM_LEFT]], type);
	// --
	mesh_push_vertex(mesh, CORNER_BOTTOM_RIGHT, face,
					 vertices[indices[CORNER_BOTTOM_RIGHT]], type);
	mesh_push_vertex(mesh, CORNER_BOTTOM_LEFT, face,
					 vertices[indices[CORNER_BOTTOM_LEFT]], type);
	mesh_push_vertex(mesh, CORNER_TOP_RIGHT, face,
					 vertices[indices[CORNER_TOP_RIGHT]], type);
}

void generate_chunk_mesh(Chunk * chunk, Map * map, Texture * tilemap)
{
	chunk->dirty = false;
	if (chunk->mesh) {
		mesh_terminate(chunk->mesh);
	}
	if (chunk->empty) {
		chunk->mesh = NULL;
		return;
	}

	chunk->mesh = calloc(sizeof(mesh_t), 1);
	resize_mesh(chunk->mesh, 512);
	chunk->mesh->texture = tilemap;

	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			for (int k = 0; k < CHUNK_SIZE; ++k) {
				int bx = i + chunk->x * CHUNK_SIZE,
				  by = j + chunk->y * CHUNK_SIZE,
				  bz = k + chunk->z * CHUNK_SIZE;
				Block *b = chunk->blocks + INCHUNK_INDEX(i, j, k);
				if (b->type == 0) {
					continue;
				}
				Block *neighbourhood[6] = { NULL };
				get_neighbourhood(map, bx, by, bz, neighbourhood);
				Block *right = neighbourhood[NEIGHBOUR_RIGHT], *left =
				  neighbourhood[NEIGHBOUR_LEFT], *up =
				  neighbourhood[NEIGHBOUR_UP], *down =
				  neighbourhood[NEIGHBOUR_DOWN], *front =
				  neighbourhood[NEIGHBOUR_FRONT], *back =
				  neighbourhood[NEIGHBOUR_BACK];
				if (left && left->type == 0) {
					push_face(chunk->mesh, i, j, k, FACE_LEFT, b->type);
				}

				if (right && right->type == 0) {
					push_face(chunk->mesh, i, j, k, FACE_RIGHT, b->type);
				}


				if (front && front->type == 0) {
					push_face(chunk->mesh, i, j, k, FACE_FRONT, b->type);
				}


				if ((back && back->type == 0)) {
					push_face(chunk->mesh, i, j, k, FACE_BACK, b->type);
				}

				if ((down && down->type == 0)) {
					push_face(chunk->mesh, i, j, k, FACE_DOWN, b->type);
				}

				if ((up && up->type == 0)) {
					push_face(chunk->mesh, i, j, k, FACE_UP, b->type);
				}
			}
		}
	}
	mesh_load(chunk->mesh);
}
