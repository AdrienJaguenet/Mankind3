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

void push_face(mesh_t * mesh, vec3_t a, vec3_t b, EFace face, int type)
{
	vec3_t vertices[8] = {
		vec3(a.x, a.y, a.z),
		vec3(a.x, a.y, b.z),
		vec3(a.x, b.y, a.z),
		vec3(a.x, b.y, b.z),
		vec3(b.x, a.y, a.z),
		vec3(b.x, a.y, b.z),
		vec3(b.x, b.y, a.z),
		vec3(b.x, b.y, b.z),
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

		  /* Z seems to be inverted, therefore so are FRONT and BACK */
	  case FACE_FRONT:
		  indices[CORNER_TOP_LEFT] = 7;
		  indices[CORNER_TOP_RIGHT] = 3;
		  indices[CORNER_BOTTOM_LEFT] = 5;
		  indices[CORNER_BOTTOM_RIGHT] = 1;
		  break;

	  case FACE_BACK:
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

void init_corner_queue(CornerQueue * queue)
{
	queue->start = queue->end = 0;
}

bool corner_queue_is_empty(CornerQueue * queue)
{
	return queue->start == queue->end;
}

void push_corner(CornerQueue * queue, vec2_t corner)
{
	queue->corners[queue->end] = corner;
	queue->end = (queue->end + 1) % (CHUNK_SIZE * CHUNK_SIZE);
}

void pop_corner(CornerQueue * queue, vec2_t * corner)
{
	*corner = queue->corners[queue->start];
	queue->start = (queue->start + 1) % (CHUNK_SIZE * CHUNK_SIZE);
}

void generate_single_chunk_mesh_greedy(Chunk * chunk, Map * map, int lod)
{
	(void) map;
	if (chunk->mesh[lod]) {
		mesh_terminate(chunk->mesh[lod]);
	}
	if (chunk->empty) {
		chunk->mesh[lod] = NULL;
		return;
	}

	chunk->mesh[lod] = calloc(sizeof(mesh_t), 1);
	resize_mesh(chunk->mesh[lod], 512);

	int x = 0, y = 0, z = 0, *a, *b, *c, *a0, *b0, *c0, x0, y0, z0;
	vec2_t corner;
	CornerQueue queue;
	init_corner_queue(&queue);

	bool used[CHUNK_SIZE][CHUNK_SIZE];

	for (int face = 0; face < 6; ++face) {
		int direction = 1;
		(void) direction;
		/* Set a, b, c according to the face set we're rendering.
		 * c will be the "fixed" component, and a and b the moving
		 * ones. For instance, for the FRONT face, z will be the
		 * fixed component, and we will work with a and b. */
		switch (face) {
		  case FACE_FRONT:
		  case FACE_BACK:
			  a = &x;
			  a0 = &x0;
			  b = &y;
			  b0 = &y0;
			  c = &z;
			  c0 = &z0;
			  break;

		  case FACE_RIGHT:
		  case FACE_LEFT:
			  a = &z;
			  a0 = &z0;
			  b = &y;
			  b0 = &y0;
			  c = &x;
			  c0 = &x0;
			  break;

		  case FACE_UP:
		  case FACE_DOWN:
			  a = &x;
			  a0 = &x0;
			  b = &z;
			  b0 = &z0;
			  c = &y;
			  c0 = &y0;
			  break;
		}

		/* For these faces, we check in reverse (remove 1 to the fixed component) */
		switch (face) {
		  case FACE_BACK:
		  case FACE_LEFT:
		  case FACE_DOWN:
			  direction = -1;
			  break;
		}
		for (*c = 0; *c < CHUNK_SIZE; ++*c) {
			*c0 = *c;
			push_corner(&queue, vec2(0, 0));
			for (int i = 0; i < CHUNK_SIZE; ++i) {
				for (int j = 0; j < CHUNK_SIZE; ++j) {
					used[i][j] = false;
				}
			}
			while (!corner_queue_is_empty(&queue)) {
				pop_corner(&queue, &corner);
				/* We start at this corner */
				*a = *a0 = corner.x;
				*b = *b0 = corner.y;
				if (used[(int) corner.x][(int) corner.y]) {
					continue;
				}
				Block *block = get_block_or_null(map, x + chunk->x * CHUNK_SIZE,
												 y + chunk->y * CHUNK_SIZE,
												 z + chunk->z * CHUNK_SIZE, 0);
				/* We increase momentarily the fixed axis to find the block in front of us */
				*c += direction;
				Block *front = get_block_or_null(map, x + chunk->x * CHUNK_SIZE,
												 y + chunk->y * CHUNK_SIZE,
												 z + chunk->z * CHUNK_SIZE, 0);
				*c -= direction;

				int square_type;
				if (!front || front->type != 0) {
					/* No need to draw this */
					square_type = -1;
				} else {
					square_type = block->type;	// <- we will try to form a rectangle with this type
				}

				/* We go as far as possible on the `a` axis */
				for (; *a < CHUNK_SIZE; ++*a) {
					Block *block =
					  get_block_or_null(map, x + chunk->x * CHUNK_SIZE,
										y + chunk->y * CHUNK_SIZE,
										z + chunk->z * CHUNK_SIZE, 0);

					int type = block->type;
					/* fetch the front cube */
					*c += direction;
					Block *front =
					  get_block_or_null(map, x + chunk->x * CHUNK_SIZE,
										y + chunk->y * CHUNK_SIZE,
										z + chunk->z * CHUNK_SIZE, 0);
					*c -= direction;
					if (!front || front->type != 0) {
						/* covered front */
						type = -1;
					}
					/* We stumbled upon a block with the incorrect type: we remove 1 to `a` (last correct position)  */
					if (type != square_type || used[*a][*b]) {
						break;
					}
				}
				*a -= 1;
				bool found = false;
				for (; *b < CHUNK_SIZE && !found; ++*b) {
					for (int l = *a0; l <= *a; ++l) {
						int old_a = *a;
						*a = l;
						Block *block =
						  get_block_or_null(map, x + chunk->x * CHUNK_SIZE,
											y + chunk->y * CHUNK_SIZE,
											z + chunk->z * CHUNK_SIZE, 0);
						int type = block->type;

						/* Same as above, we increase *c to find the block in front of us */
						*c += direction;
						Block *front =
						  get_block_or_null(map, x + chunk->x * CHUNK_SIZE,
											y + chunk->y * CHUNK_SIZE,
											z + chunk->z * CHUNK_SIZE, 0);
						*c -= direction;
						*a = old_a;
						if (!front || front->type != 0) {
							type = -1;
						}
						if (type != square_type || used[l][*b]) {
							found = true;
							break;
						}
					}
				}
				*b -= 1;
				if (found) {
					*b -= 1;
				}

				/* set cells as used */
				for (int m = *a0; m <= *a; ++m) {
					for (int n = *b0; n <= *b; ++n) {
						used[m][n] = true;
					}
				}

				/* We finally have our rectangle. */
				if (square_type > 0) {
					push_face(chunk->mesh[lod], vec3(x0, y0, z0),
							  vec3(x + 1, y + 1, z + 1), face, square_type);
				}

				for (; *b < CHUNK_SIZE - 1; ++*b) {
					if (!used[*a0][*b + 1]) {
						push_corner(&queue, vec2(*a0, *b + 1));
						break;
					}
				}
				for (; *a < CHUNK_SIZE - 1; ++*a) {
					if (!used[*a + 1][*b0]) {
						push_corner(&queue, vec2(*a + 1, *b0));
						break;
					}
				}
			}
			for (int i = 0; i < CHUNK_SIZE; ++i) {
				for (int j = 0; j < CHUNK_SIZE; ++j) {
					if (!used[i][j]) {
						WARN("Failed to cover");
					}
				}
			}
		}
	}
	mesh_load(chunk->mesh[lod]);
}

void generate_chunk_mesh(Chunk * chunk, Map * map)
{
	chunk->dirty = false;
	generate_single_chunk_mesh_greedy(chunk, map, 0);
}
