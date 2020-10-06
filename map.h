#pragma once
#include <stdint.h>
#include "position.h"
#include "mesh.h"

#define BLOCK_TYPES_NO 4

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
	bool pending_meshgen;
} Chunk;

typedef struct {
	Chunk **chunks;
	int chunks_no;
} Map;

void randomly_populate(Map * m, Chunk * chunk);

void set_Chunk_block_type(Chunk * c, int x, int y, int z, int type);

void gen_Chunk_LOD(Chunk * c);


void insert_chunk(Map * map, int px, int py, int pz, Chunk * chunk);

Chunk *get_chunk_or_null(Map * map, int px, int py, int pz);

Block *get_block_or_null(Map * map, int bx, int by, int bz, int lod);

void get_neighbourhood(Map * map, int x, int y, int z, Block * neighbours[6],
					   int lod);

Chunk *new_Chunk(Map * map, int px, int py, int pz);

void delete_Map(Map * map);
