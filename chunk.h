#pragma once

#include "mesh.h"
#include "position.h"

typedef struct {
	int type;
} Block;

typedef struct {
	Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
	int x, y, z;
	mesh_t *mesh;
} Chunk;
