#pragma once
#include "map.h"
#include "texture.h"

#define INDEX(x, y, z) ((x) * (CHUNK_SIZE+1) * (CHUNK_SIZE+1) + (y) * (CHUNK_SIZE+1) + (z))

typedef struct {
	vec2_t corners[CHUNK_SIZE * CHUNK_SIZE];
	int start;
	int end;
} CornerQueue;

void init_corner_queue(CornerQueue * queue);
bool corner_queue_is_empty(CornerQueue * queue);
void push_corner(CornerQueue * queue, vec2_t corner);
void pop_corner(CornerQueue * queue, vec2_t * corner);

void generate_chunk_mesh(Chunk * chunk, Map * map);
