#include "map.h"
#include <stdlib.h>
#include "position.h"

Key get_key(int px, int py, int pz)
{
	return (Key) px << 40 | (Key) py << 20 | (Key) pz;
}

MapBucket *new_MapBucket(Key key, Chunk * chunk)
{
	MapBucket *bucket = malloc(sizeof(MapBucket));
	bucket->larger = NULL;
	bucket->smaller = NULL;
	bucket->key = key;
	bucket->chunk = chunk;
	return bucket;
}

void btree_insert_chunk(MapBucket * root, Key key, Chunk * chunk)
{
	if (key > root->key) {
		if (root->larger) {
			btree_insert_chunk(root->larger, key, chunk);
		} else {
			root->larger = new_MapBucket(key, chunk);
		}
	} else {
		if (root->smaller) {
			btree_insert_chunk(root->smaller, key, chunk);
		} else {
			root->smaller = new_MapBucket(key, chunk);
		}
	}
}

Chunk *btree_get_chunk(MapBucket * root, Key key)
{
	if (key == root->key) {
		return root->chunk;
	} else if (key > root->key) {
		if (root->larger) {
			return btree_get_chunk(root->larger, key);
		} else {
			return NULL;
		}
	} else {
		if (root->smaller) {
			return btree_get_chunk(root->smaller, key);
		} else {
			return NULL;
		}
	}
}

void insert_chunk(Map * map, int px, int py, int pz, Chunk * chunk)
{
	Key key = get_key(px, py, pz);
	if (!map->root) {
		map->root = new_MapBucket(key, chunk);
	}
	btree_insert_chunk(map->root, key, chunk);
	map->chunks_no++;
}

Chunk *get_chunk_or_null(Map * map, int px, int py, int pz)
{
	Key key = get_key(px, py, pz);
	if (map->root) {
		return btree_get_chunk(map->root, key);
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
	insert_chunk(map, px, py, pz, chunk);
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

void btree_foreach(MapBucket * bc, void (*fun)(Chunk * c, void *custom),
				   void *custom_arg)
{
	fun(bc->chunk, custom_arg);
	if (bc->larger) {
		btree_foreach(bc->larger, fun, custom_arg);
	}
	if (bc->smaller) {
		btree_foreach(bc->smaller, fun, custom_arg);
	}
}

void for_each_Chunk(Map * map, void (*fun)(Chunk * c, void *custom),
					void *custom_arg)
{
	if (!map->root) {
		return;
	} else {
		btree_foreach(map->root, fun, custom_arg);
	}
}

void gen_Map(Map* map)
{
	for (int i = 0; i < 4; ++i) {
	  for (int j = 0; j < 1; ++j) {
		for (int k = 0; k < 4; ++k) {
			Chunk *c = new_Chunk(map, i, j, k);
			randomly_populate(c);
		}
	  }
	}
}

