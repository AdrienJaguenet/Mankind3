#include "map.h"
#include <stdlib.h>
#include "position.h"

HashMap *get_Chunk_storage(Map * map, int cx, int cy, int lod)
{
	HashMap *hashmap = &map->chunks[lod];
	HashMap *new_map = get_from_HashMap(hashmap, cx);
	if (!new_map) {
		new_map = malloc(sizeof(HashMap));
		init_HashMap(new_map);
		insert_into_HashMap(hashmap, cx, new_map);
	}
	hashmap = new_map;
	new_map = get_from_HashMap(hashmap, cy);
	if (!new_map) {
		new_map = malloc(sizeof(HashMap));
		init_HashMap(new_map);
		insert_into_HashMap(hashmap, cy, new_map);
	}

	hashmap = new_map;
	return hashmap;
}

void insert_chunk(Map * map, int px, int py, int pz, Chunk * chunk)
{
	insert_into_HashMap(get_Chunk_storage(map, px, py, chunk->lod), pz, chunk);
	map->chunks_no++;
}

Chunk *get_chunk_or_null(Map * map, int px, int py, int pz, int lod)
{
	if (map->last_access) {
		if (map->last_access->x == px &&
			map->last_access->y == py &&
			map->last_access->z == pz && map->last_access->lod == lod) {
			return map->last_access;
		}
	}
	Chunk *c = get_from_HashMap(get_Chunk_storage(map, px, py, lod), pz);
	map->last_access = c;
	return c;
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

void set_block_type(Map * map, int x, int y, int z, int lod, int type)
{
	int cx, cy, cz, bx, by, bz;
	get_chunk_pos(x, y, z, &cx, &cy, &cz);
	get_pos_in_chunk(x, y, z, &bx, &by, &bz, lod);
	Chunk *c = get_chunk_or_null(map, cx, cy, cz, lod);

	if (c != NULL) {
		set_Chunk_block_type(map, c, bx, by, bz, type);
	}
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

typedef struct {
	void *extra;
	Map *map;
	void (*function)(Map * m, Chunk * c, void *extra);
} ForEachBundle;

void for_each_Chunk_final(Key key, void *value, void *extra)
{
	(void) key;
	ForEachBundle *bundle = (ForEachBundle *) extra;
	Chunk *chunk = (Chunk *) value;
	bundle->function(bundle->map, chunk, bundle->extra);
}

void for_each_Chunk_mapZ(Key key, void *value, void *extra)
{
	(void) key;
	for_each_in_HashMap(value, for_each_Chunk_final, extra);
}

void for_each_Chunk_mapY(Key key, void *value, void *extra)
{
	(void) key;
	for_each_in_HashMap(value, for_each_Chunk_mapZ, extra);
}

void for_each_Chunk(Map * map, int lod,
					void (*fn)(Map * m, Chunk * c, void *extra), void *extra)
{
	ForEachBundle bundle = { extra, map, fn };
	for_each_in_HashMap(&map->chunks[lod], for_each_Chunk_mapY, &bundle);
}


void delete_Chunk(Chunk * chunk)
{
	if (chunk->mesh) {
		mesh_terminate(chunk->mesh);
	}
	free(chunk);
}

void del_HashMap_Chunk(Key key, void *chunk, void *extra)
{
	(void) extra;
	(void) key;
	delete_Chunk(chunk);
}

void del_HashMap_Y(Key key, void *map_Z, void *extra)
{
	(void) extra;
	(void) key;
	for_each_in_HashMap(map_Z, del_HashMap_Chunk, NULL);
}

void del_HashMap_X(Key key, void *map_Y, void *extra)
{
	(void) extra;
	(void) key;
	for_each_in_HashMap(map_Y, del_HashMap_Y, NULL);
}


void delete_Map(Map * map)
{
	for (int lod = 0; lod < MAX_LOD; ++lod) {
		for_each_in_HashMap(&map->chunks[lod], del_HashMap_X, NULL);
	}
}

float get_height(int x, int z, u_int8_t * permutations)
{
	return fbm2((x + MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS) / 400.f,
				(z + MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS) / 400.f, 6,
				permutations);
}

float get_3d(int x, int y, int z, u_int8_t * permutations)
{
	return fbm3((x + MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS) / 40.f,
				(y + MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS) / 20.f,
				(z + MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS) / 40.f, 1,
				permutations);
}

float get_wetness(int x, int z, u_int8_t * permutations)
{
	return fbm2((x + MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS) / 100.f,
				(z + MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS) / 100.f, 2,
				permutations);
}

float get_heat(int x, int z, u_int8_t * permutations)
{
	return fbm2((x + MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS) / 200.f,
				(z + MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS) / 200.f, 2,
				permutations);
}

void randomly_populate(Map * m, Chunk * chunk)
{
	int base_height = chunk->y * CHUNK_SIZE;
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int k = 0; k < CHUNK_SIZE; ++k) {
			float height_noise =
			  get_height(i + chunk->x * CHUNK_SIZE, k + chunk->z * CHUNK_SIZE,
						 m->height_perm);
			float wet_noise = get_wetness(i + chunk->x * CHUNK_SIZE,
										  k + chunk->z * CHUNK_SIZE,
										  m->wet_perm);
			float heat_noise = get_heat(i + chunk->x * CHUNK_SIZE,
										k + chunk->z * CHUNK_SIZE,
										m->heat_perm);

			int height = height_noise * TERRAIN_HEIGHT;

			for (int j = 0; j < CHUNK_SIZE; ++j) {
				int type = 0;
				Biome *biome =
				  get_Biome_from_noise_or_null(&(NoiseSet) { height_noise,
											   heat_noise, wet_noise,
											   }, &m->biome_table);
				(void) biome;
				if (j + base_height < height - 4) {
					type = 1;
				} else if (j + base_height < height - 1) {
					type = 3;
				} else if (biome != NULL && j + base_height == height - 1
						   && biome->type != 6) {
					type = biome->type;
				} else if (biome != NULL && biome->type == 6
						   && j + base_height + 2 >= height
						   && j + base_height <=
						   biome->descriptor.height_max * TERRAIN_HEIGHT) {
					/* I think all of the above is largely bruteforced. I don't know why it bugs out without `base_height + 2'. */
					type = 6;
				}

				float threed =
				  get_3d(i + chunk->x * CHUNK_SIZE, j + chunk->y * CHUNK_SIZE,
						 k + chunk->z * CHUNK_SIZE, m->height_perm);
				if (threed < -0.21 && type != 6) {	// Do not cut into water
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
		neighbour = get_chunk_or_null(map, c->x, c->y, c->z + 1, c->lod);
		if (neighbour) {
			neighbour->dirty = true;
		}
	}
	b->has_void = type == 0;
	b->type = type;
}
