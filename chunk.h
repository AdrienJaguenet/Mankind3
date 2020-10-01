#pragma once

typedef struct {
	int type;
} Block;

typedef struct {
	Block blocks[16 * 16 * 16];
	int x, y, z;
} Chunk;
