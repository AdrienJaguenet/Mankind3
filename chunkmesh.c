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

void generate_single_chunk_mesh(Chunk * chunk, Map * map, int lod)
{
	if (chunk->mesh[lod]) {
		mesh_terminate(chunk->mesh[lod]);
	}
	if (chunk->empty) {
		chunk->mesh[lod] = NULL;
		return;
	}

	chunk->mesh[lod] = calloc(sizeof(mesh_t), 1);
	resize_mesh(chunk->mesh[lod], 512);

	/* i, j, k: fake coordinates, must be multiplied by 2^LOD to get in-chunk coordinates */
	for (int i = 0; i < CHUNK_SIZE >> lod; ++i) {
		for (int j = 0; j < CHUNK_SIZE >> lod; ++j) {
			for (int k = 0; k < CHUNK_SIZE >> lod; ++k) {

				/* Real world coordinates of the top left front block of the cube of size 2^LOD that we are checking */
				int bx = (i << lod) + (chunk->x * CHUNK_SIZE),
				  by = (j << lod) + (chunk->y * CHUNK_SIZE),
				  bz = (k << lod) + (chunk->z * CHUNK_SIZE);
				int type = 0;
				for (int m = 0; m < 1 << lod && !type; ++m) {
					for (int n = 0; n < 1 << lod && !type; ++n) {
						for (int o = 0; o < 1 << lod; ++o) {
							Block *b =
							  chunk->blocks + INCHUNK_INDEX((i << lod) + m,
															(j << lod) + n,
															(k << lod) + o);
							if (b->type != 0) {
								type = b->type;
								break;
							}
						}
					}
				}
				if (type == 0) {
					continue;
				}
				Block *block = NULL;
				bool neighbour_empty;
				neighbour_empty = false;
				for (int m = 0; m < 1 << lod; ++m) {
					for (int n = 0; n < 1 << lod && !neighbour_empty; ++n) {
						block = get_block_or_null(map, bx - 1, by + n, bz + n);
						if (block && block->type == 0) {
							neighbour_empty = true;
							break;
						}
					}
				}
				if (neighbour_empty) {
					push_face(chunk->mesh[lod], i, j, k, FACE_LEFT, type);
				}

				neighbour_empty = false;
				for (int m = 0; m < 1 << lod; ++m) {
					for (int n = 0; n < 1 << lod && !neighbour_empty; ++n) {
						block =
						  get_block_or_null(map, bx + (1 << lod), by + n,
											bz + n);
						if (block && block->type == 0) {
							neighbour_empty = true;
							break;
						}
					}
				}
				if (neighbour_empty) {
					push_face(chunk->mesh[lod], i, j, k, FACE_RIGHT, type);
				}

				neighbour_empty = false;
				for (int m = 0; m < 1 << lod; ++m) {
					for (int n = 0; n < 1 << lod && !neighbour_empty; ++n) {
						block = get_block_or_null(map, bx + m, by + n, bz - 1);
						if (block && block->type == 0) {
							neighbour_empty = true;
							break;
						}
					}
				}
				if (neighbour_empty) {
					push_face(chunk->mesh[lod], i, j, k, FACE_FRONT, type);
				}

				neighbour_empty = false;
				for (int m = 0; m < 1 << lod; ++m) {
					for (int n = 0; n < 1 << lod && !neighbour_empty; ++n) {
						block =
						  get_block_or_null(map, bx + m, by + n,
											bz + (1 << lod));
						if (block && block->type == 0) {
							neighbour_empty = true;
							break;
						}
					}
				}
				if (neighbour_empty) {
					push_face(chunk->mesh[lod], i, j, k, FACE_BACK, type);
				}

				neighbour_empty = false;
				for (int m = 0; m < 1 << lod; ++m) {
					for (int n = 0; n < 1 << lod && !neighbour_empty; ++n) {
						block = get_block_or_null(map, bx + m, by - 1, bz + n);
						if (block && block->type == 0) {
							neighbour_empty = true;
							break;
						}
					}
				}
				if (neighbour_empty) {
					push_face(chunk->mesh[lod], i, j, k, FACE_DOWN, type);
				}

				neighbour_empty = false;
				for (int m = 0; m < 1 << lod; ++m) {
					for (int n = 0; n < 1 << lod && !neighbour_empty; ++n) {
						block = get_block_or_null(map, bx + m, by + 1, bz + n);
						if (block && block->type == 0) {
							neighbour_empty = true;
							break;
						}
					}
				}
				if (neighbour_empty) {
					push_face(chunk->mesh[lod], i, j, k, FACE_UP, type);
				}
			}
		}
	}
	mesh_load(chunk->mesh[lod]);

}

void generate_chunk_mesh(Chunk * chunk, Map * map)
{
	chunk->dirty = false;
	for (int i = 0; i < MAX_LOD; ++i) {
		generate_single_chunk_mesh(chunk, map, i);
	}
}
