#pragma once
#include <stdint.h>
#include "position.h"
#include "mesh.h"
#include "noise.h"

#define BLOCK_TYPES_NO 4
#define SEED 1337

typedef enum {
	PENDING_MESHGEN,
	PENDING_TERRAIN
} ChunkPending;

typedef struct {
	int type;
	bool has_void;
} Block;

typedef struct {
	Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE][MAX_LOD];
	int x, y, z;
	mesh_t *mesh[MAX_LOD];
	bool empty;
	bool dirty;
	bool pending[8];
} Chunk;

typedef struct {
	Chunk **chunks;
	int chunks_no;
	int *hash;
} Map;

void randomly_populate(Map * m, Chunk * chunk);

void set_Chunk_block_type(Map * map, Chunk * c, int x, int y, int z, int type);

void gen_Chunk_LOD(Chunk * c);


void insert_chunk(Map * map, int px, int py, int pz, Chunk * chunk);

Chunk *get_chunk_or_null(Map * map, int px, int py, int pz);

Block *get_block_or_null(Map * map, int bx, int by, int bz, int lod);

void get_neighbourhood(Map * map, int x, int y, int z, Block * neighbours[6],
					   int lod);

Chunk *new_Chunk(Map * map, int px, int py, int pz);

void delete_Map(Map * map);

int get_height(int x, int z, int *hash);
