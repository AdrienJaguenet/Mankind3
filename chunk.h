#pragma once
#include "mesh.h"

typedef struct {
	int type;
} Block;

typedef struct {
	Block blocks[16 * 16 * 16];
	int x, y, z;
	mesh_t *mesh;
} Chunk;
