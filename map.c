#include "map.h"
#include <stdlib.h>
#include "position.h"

void insert_chunk(Map * map, int px, int py, int pz, Chunk * chunk)
{
	int chunk_pos = CHUNK_INMAP(px, py, pz);
	if (chunk_pos >= 0 && chunk_pos < MAX_CHUNKS_NO) {
		map->chunks[chunk->lod][chunk_pos] = chunk;
		map->chunks_no++;
	}
}

Chunk *get_chunk_or_null(Map * map, int px, int py, int pz, int lod)
{
	int chunk_pos = CHUNK_INMAP(px, py, pz);
	if (chunk_pos >= 0 && chunk_pos < MAX_CHUNKS_NO) {
		Chunk *c = map->chunks[lod][chunk_pos];
		return c;
	} else {
		return NULL;
	}
}

Chunk *new_Chunk(Map * map, int px, int py, int pz, int lod)
{
	Chunk *chunk = calloc(sizeof(Chunk), 1);
	chunk->x = px;
	chunk->y = py;
	chunk->z = pz;
	chunk->lod = lod;
	chunk->mesh = NULL;
	chunk->empty = true;
	insert_chunk(map, px, py, pz, chunk);
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			for (int k = -1; k <= 1; ++k) {
				Chunk *neighbour =
				  get_chunk_or_null(map, px + i, py + j, pz + k, lod);
				if (neighbour) {
					neighbour->dirty = true;
				}
			}
		}
	}
	return chunk;
}

Block *get_block_or_null(Map * map, int x, int y, int z, int lod)
{
	int cx, cy, cz, bx, by, bz;
	get_chunk_pos(x, y, z, &cx, &cy, &cz);
	get_pos_in_chunk(x, y, z, &bx, &by, &bz, lod);
	Chunk *c = get_chunk_or_null(map, cx, cy, cz, lod);
	int block_index = INCHUNK_INDEX(bx, by, bz);
	if (c == NULL) {
		return NULL;
	}
	return &c->blocks[block_index];
}

void get_neighbourhood(Map * map, int x, int y, int z, Block * neighbours[6],
					   int lod)
{
	neighbours[NEIGHBOUR_LEFT] = get_block_or_null(map, x - 1, y, z, lod);
	neighbours[NEIGHBOUR_RIGHT] = get_block_or_null(map, x + 1, y, z, lod);
	neighbours[NEIGHBOUR_FRONT] = get_block_or_null(map, x, y, z - 1, lod);
	neighbours[NEIGHBOUR_BACK] = get_block_or_null(map, x, y, z + 1, lod);
	neighbours[NEIGHBOUR_UP] = get_block_or_null(map, x, y + 1, z, lod);
	neighbours[NEIGHBOUR_DOWN] = get_block_or_null(map, x, y - 1, z, lod);
}

void delete_Map(Map * map)
{
	for (int lod = 0; lod < MAX_LOD; ++lod) {
		if (map->chunks[lod]) {
			for (int j = 0; j < MAX_CHUNKS_NO; ++j) {
				if (map->chunks[lod][j] && map->chunks[lod][j]->mesh) {
					mesh_terminate(map->chunks[lod][j]->mesh);
				}
				free(map->chunks[lod][j]);
			}
		}
		free(map->chunks[lod]);
	}
}

int get_height(int x, int z, int *permutations)
{
	return fractal2(x, z, permutations) * 24.f;
}

void randomly_populate(Map * m, Chunk * chunk)
{
	srand(SEED);

	int base_height = chunk->y * CHUNK_SIZE;
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int k = 0; k < CHUNK_SIZE; ++k) {
			int height =
			  get_height(i + chunk->x * CHUNK_SIZE, k + chunk->z * CHUNK_SIZE,
						 m->permutations);
			for (int j = 0; j < CHUNK_SIZE; ++j) {
				int type = 0;
				if (j + base_height < height - 4) {
					type = 1;
				} else if (j + base_height < height - 1) {
					type = 3;
				} else if (j + base_height == height - 1) {
					type = 2;
				}

				if (fractal3(i + chunk->x * CHUNK_SIZE,
							 j + chunk->y * CHUNK_SIZE,
							 k + chunk->z * CHUNK_SIZE,
							 m->permutations) < -0.5) {
					type = 0;
				}

				set_Chunk_block_type(m, chunk, i, j, k, type);
			}
		}
	}
}

void set_Chunk_block_type(Map * map, Chunk * c, int x, int y, int z, int type)
{
	c->dirty = true;
	Block *b = &c->blocks[INCHUNK_INDEX(x, y, z)];
	if (type) {
		c->empty = false;
	}
	Chunk *neighbour;
	if (x == 0) {
		neighbour = get_chunk_or_null(map, c->x - 1, c->y, c->z, c->lod);
		if (neighbour) {
			neighbour->dirty = true;
		}
	}
	if (y == 0) {
		neighbour = get_chunk_or_null(map, c->x, c->y - 1, c->z, c->lod);
		if (neighbour) {
			neighbour->dirty = true;
		}
	}
	if (z == 0) {
		neighbour = get_chunk_or_null(map, c->x, c->y, c->z - 1, c->lod);
		if (neighbour) {
			neighbour->dirty = true;
		}
	}
	if (x == CHUNK_SIZE - 1) {
		neighbour = get_chunk_or_null(map, c->x + 1, c->y, c->z, c->lod);
		if (neighbour) {
			neighbour->dirty = true;
		}
	}
	if (y == CHUNK_SIZE - 1) {
		neighbour = get_chunk_or_null(map, c->x, c->y + 1, c->z, c->lod);
		if (neighbour) {
			neighbour->dirty = true;
		}
	}
	if (z == CHUNK_SIZE - 1) {
		neighbour = get_chunk_or_null(map, c->x, c->y + 1, c->z, c->lod);
		if (neighbour) {
			neighbour->dirty = true;
		}
	}
	b->has_void = type == 0;
	b->type = type;
}
