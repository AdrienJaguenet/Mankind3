#pragma once
#include "chunk.h"
#include <stdint.h>

typedef struct {
	Chunk **chunks;
	int chunks_no;
} Map;

void insert_chunk(Map * map, int px, int py, int pz, Chunk * chunk);

Chunk *get_chunk_or_null(Map * map, int px, int py, int pz);

Block *get_block_or_null(Map * map, int bx, int by, int bz);

Chunk *new_Chunk(Map * map, int px, int py, int pz);

void get_neighbourhood(Map * map, int x, int y, int z, Block * neighbours[6]);

void delete_Map(Map * map);
