#pragma once

#include "mesh.h"
#include "position.h"

typedef struct {
	int type;
} Block;

typedef struct {
	Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
	int x, y, z;
	mesh_t *mesh[MAX_LOD];
	bool empty;
	bool dirty;
	bool pending_meshgen;
} Chunk;

void randomly_populate(Chunk * chunk);

void set_Chunk_block_type(Chunk * c, int x, int y, int z, int type);
