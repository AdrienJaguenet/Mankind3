#pragma once
#include <stdint.h>

#include "biome.h"
#include "position.h"
#include "mesh.h"
#include "noise.h"
#include "hashmap.h"

#define BLOCK_TYPES_NO 8
#define SEED 666
#define TERRAIN_HEIGHT 128

typedef enum {
	PENDING_MESHGEN,
	PENDING_TERRAIN,
	PENDING_RENDER
} ChunkPending;

typedef struct {
	int type;
	bool has_void;
} Block;

typedef struct {
	Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
	int x, y, z, lod;
	mesh_t *mesh;
	bool empty;
	bool dirty;
	bool pending[8];
} Chunk;

typedef struct {
	HashMap chunks[MAX_LOD];
	Chunk *last_access;
	int chunks_no;
	u_int8_t *height_perm, *wet_perm, *heat_perm;
	BiomeTable biome_table;
} Map;

void randomly_populate(Map * m, Chunk * chunk);

void set_Chunk_block_type(Map * map, Chunk * c, int x, int y, int z, int type);

void gen_Chunk_LOD(Chunk * c);

void for_each_Chunk(Map * map, int lod,
					void (*fn)(Map * m, Chunk * c, void *extra), void *extra);

void insert_chunk(Map * map, int px, int py, int pz, Chunk * chunk);

Chunk *get_chunk_or_null(Map * map, int px, int py, int pz, int lod);

Block *get_block_or_null(Map * map, int bx, int by, int bz, int lod);

void set_block_type(Map * map, int x, int y, int z, int lod, int type);

void get_neighbourhood(Map * map, int x, int y, int z, Block * neighbours[6],
					   int lod);

Chunk *new_Chunk(Map * map, int px, int py, int pz, int lod);

void delete_Chunk(Chunk * chunk);

void delete_Map(Map * map);
