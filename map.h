#pragma once
#include "chunk.h"
#include <stdint.h>

#define MAP_SIZE_IN_CHUNKS 32
#define MAX_CHUNKS_NO MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS
#define CHUNK_INMAP(x, y, z)\
  (((x) + MAP_SIZE_IN_CHUNKS / 2) * MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS +\
   ((y) + MAP_SIZE_IN_CHUNKS / 2) * MAP_SIZE_IN_CHUNKS +\
   ((z) + MAP_SIZE_IN_CHUNKS / 2))

typedef struct {
	Chunk *chunks[32 * 32 * 32];
	int chunks_no;
} Map;

void insert_chunk(Map * map, int px, int py, int pz, Chunk * chunk);

Chunk *get_chunk_or_null(Map * map, int px, int py, int pz);

Block *get_block_or_null(Map * map, int bx, int by, int bz);

Chunk *new_Chunk(Map * map, int px, int py, int pz);

void get_neighbourhood(Map * map, int x, int y, int z, Block * neighbours[6]);

void for_each_Chunk(Map * map, void (*fun)(Chunk * c, void *custom),
					void *custom_arg);

void gen_Map(Map * map);
