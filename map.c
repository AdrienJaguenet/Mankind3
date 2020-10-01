#include "map.h"
#include <stdlib.h>
#include "position.h"

int64_t get_key(int px, int py, int pz)
{
	return (int64_t) px << 40 | (int64_t) py << 20 | (int64_t) pz;
}

MapBucket *new_MapBucket(int key, Chunk * chunk)
{
	MapBucket *bucket = malloc(sizeof(MapBucket));
	bucket->larger = NULL;
	bucket->smaller = NULL;
	bucket->key = key;
	bucket->chunk = chunk;
	return bucket;
}

void btree_insert_chunk(MapBucket * root, int key, Chunk * chunk)
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

Chunk *btree_get_chunk(MapBucket * root, int key)
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
	if (map->root) {
		btree_insert_chunk(map->root, get_key(px, py, pz), chunk);
	} else {
		map->root = new_MapBucket(get_key(px, py, pz), chunk);
	}
	map->chunks_no++;
}

Chunk *get_chunk_or_null(Map * map, int px, int py, int pz)
{
	if (map->root) {
		return btree_get_chunk(map->root, get_key(px, py, pz));
	} else {
		return NULL;
	}
}

Chunk *new_Chunk(Map * map, int px, int py, int pz)
{
	Chunk *chunk = calloc(sizeof(Chunk), 0);
	insert_chunk(map, px, py, pz, chunk);
	return chunk;
}
