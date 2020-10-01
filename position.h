#pragma once

#define CHUNK_SIZE 16

typedef enum {
	NEIGHBOUR_LEFT = 0,
	NEIGHBOUR_RIGHT = 1,
	NEIGHBOUR_UP = 2,
	NEIGHBOUR_DOWN = 3,
	NEIGHBOUR_FRONT = 4,
	NEIGHBOUR_BACK = 5
} NeighbourhoodPosition;

void get_chunk_pos(int px, int py, int pz, int *cx, int *cy, int *cz);
void get_pos_in_chunk(int px, int py, int pz, int *bx, int *by, int *bz);
