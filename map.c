#include "map.h"
#include <stdlib.h>
#include "position.h"

void insert_chunk(Map * map, int px, int py, int pz, Chunk * chunk)
{
	int chunk_pos = CHUNK_INMAP(px, py, pz);
	if (chunk_pos < MAX_CHUNKS_NO) {
		map->chunks[CHUNK_INMAP(px, py, pz)] = chunk;
		map->chunks_no++;
	}
}

Chunk *get_chunk_or_null(Map * map, int px, int py, int pz)
{
	int chunk_pos = CHUNK_INMAP(px, py, pz);
	if (chunk_pos < MAX_CHUNKS_NO) {
		return map->chunks[CHUNK_INMAP(px, py, pz)];
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
	chunk->mesh = NULL;
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

Block *get_block_or_null(Map * map, int x, int y, int z)
{
	int cx, cy, cz, bx, by, bz;
	get_chunk_pos(x, y, z, &cx, &cy, &cz);
	get_pos_in_chunk(x, y, z, &bx, &by, &bz);
	Chunk *c = get_chunk_or_null(map, cx, cy, cz);
	if (c != NULL) {
		int block_index = bx * CHUNK_SIZE * CHUNK_SIZE + by * CHUNK_SIZE + bz;
		return c->blocks + block_index;
	} else {
		return NULL;
	}
}

void get_neighbourhood(Map * map, int x, int y, int z, Block * neighbours[6])
{
	neighbours[NEIGHBOUR_LEFT] = get_block_or_null(map, x - 1, y, z);
	neighbours[NEIGHBOUR_RIGHT] = get_block_or_null(map, x + 1, y, z);
	neighbours[NEIGHBOUR_UP] = get_block_or_null(map, x, y + 1, z);
	neighbours[NEIGHBOUR_DOWN] = get_block_or_null(map, x, y - 1, z);
	neighbours[NEIGHBOUR_FRONT] = get_block_or_null(map, x, y, z - 1);
	neighbours[NEIGHBOUR_BACK] = get_block_or_null(map, x, y, z + 1);
}
