#include "map.h"
#include <stdlib.h>
#include "position.h"

void insert_chunk(Map * map, int px, int py, int pz, Chunk * chunk)
{
	int chunk_pos = CHUNK_INMAP(px, py, pz);
	if (chunk_pos >= 0 && chunk_pos < MAX_CHUNKS_NO) {
		map->chunks[chunk_pos] = chunk;
		map->chunks_no++;
	}
}

Chunk *get_chunk_or_null(Map * map, int px, int py, int pz)
{
	int chunk_pos = CHUNK_INMAP(px, py, pz);
	if (chunk_pos >= 0 && chunk_pos < MAX_CHUNKS_NO) {
		Chunk *c = map->chunks[chunk_pos];
		return c;
	} else {
		return NULL;
	}
}

Chunk *new_Chunk(Map * map, int px, int py, int pz)
{
	Chunk *chunk = calloc(sizeof(Chunk), 1);
	chunk->x = px;
	chunk->y = py;
	chunk->z = pz;
	for (int i = 0; i < MAX_LOD; ++i) {
		chunk->mesh[i] = NULL;
	}
	chunk->empty = true;
	insert_chunk(map, px, py, pz, chunk);
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			for (int k = -1; k <= 1; ++k) {
				Chunk *neighbour =
				  get_chunk_or_null(map, px + i, py + j, pz + k);
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
	Chunk *c = get_chunk_or_null(map, cx >> lod, cy >> lod, cz >> lod);
	int block_index = INCHUNK_INDEX(bx, by, bz);
	if (c == NULL) {
		return NULL;
	}
	return c->blocks[block_index] + lod;
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
	for (int i = 0; i < MAX_CHUNKS_NO; ++i) {
		if (map->chunks[i]) {
			for (int j = 0; j < MAX_LOD; ++j) {
				if (map->chunks[i]->mesh[j]) {
					mesh_terminate(map->chunks[i]->mesh[j]);
				}
			}
			free(map->chunks[i]);
		}
	}
}

int get_height(int x, int z, int *hash)
{
	return fractal(x, z, hash) * 64.f;
}

void randomly_populate(Map * m, Chunk * chunk)
{
	srand(SEED);

	int base_height = chunk->y * CHUNK_SIZE;
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int k = 0; k < CHUNK_SIZE; ++k) {
			int height =
			  get_height(i + chunk->x * CHUNK_SIZE, k + chunk->z * CHUNK_SIZE,
						 m->hash);
			for (int j = 0; j < CHUNK_SIZE; ++j) {
				int type = 0;
				if (j + base_height < height - 4) {
					type = 1;
				} else if (j + base_height < height - 1) {
					type = 3;
				} else if (j + base_height == height - 1) {
					type = 2;
				}

				set_Chunk_block_type(chunk, i, j, k, type);
			}
		}
	}
}

void set_Chunk_block_type(Chunk * c, int x, int y, int z, int type)
{
	c->dirty = true;
	Block *b = &c->blocks[INCHUNK_INDEX(x, y, z)][0];
	if (type) {
		c->empty = false;
	}
	b->has_void = type == 0;
	b->type = type;
}

void gen_Chunk_LOD(Chunk * c)
{
	int blocks[BLOCK_TYPES_NO];
	/* For each LOD */
	for (int lod = 1; lod < MAX_LOD; ++lod) {
		for (int i = 0; i < CHUNK_SIZE >> lod; ++i) {
			for (int j = 0; j < CHUNK_SIZE >> lod; ++j) {
				for (int k = 0; k < CHUNK_SIZE >> lod; ++k) {
					for (int l = 0; l < BLOCK_TYPES_NO; ++l) {
						blocks[l] = 0;
					}
					/* Within the cube */
					int most_common = 0;
					bool has_void = false;
					for (int m = 0; m < (1 << lod); ++m) {
						for (int n = 0; n < (1 << lod); ++n) {
							for (int o = 0; o < (1 << lod); ++o) {
								Block *b =
								  &c->blocks[INCHUNK_INDEX
											 ((i << lod) + m, (j << lod) + n,
											  (k << lod) + o)][0];
								has_void |= b->has_void;
								if (b->type) {
									blocks[b->type]++;
								}
							}
						}
					}
					for (int l = 0; l < BLOCK_TYPES_NO; ++l) {
						if (blocks[l] > blocks[most_common]) {
							most_common = l;
						}
					}
					c->blocks[INCHUNK_INDEX(i, j, k)][lod].type = most_common;
					c->blocks[INCHUNK_INDEX(i, j, k)][lod].has_void = has_void;
				}
			}
		}
	}
}
