#pragma once

#define MAP_SIZE_IN_CHUNKS 32
#define MAX_CHUNKS_NO MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS
#define BLOCK_SIZE 1.f
#define CHUNK_SIZE 16
#define INCHUNK_INDEX(x, y, z)\
  (((z) * CHUNK_SIZE * CHUNK_SIZE) + ((y) *  CHUNK_SIZE) + (x))
#define CHUNK_INMAP(x, y, z)\
  (((x) + MAP_SIZE_IN_CHUNKS / 2) * MAP_SIZE_IN_CHUNKS * MAP_SIZE_IN_CHUNKS +\
   ((y) + MAP_SIZE_IN_CHUNKS / 2) * MAP_SIZE_IN_CHUNKS +\
   ((z) + MAP_SIZE_IN_CHUNKS / 2))
#define MAX_LOD 4

typedef enum {
	FACE_LEFT = 0,
	FACE_RIGHT = 1,
	FACE_UP = 2,
	FACE_DOWN = 3,
	FACE_FRONT = 4,
	FACE_BACK = 5
} EFace;

typedef enum {
	CORNER_TOP_LEFT = 0,
	CORNER_TOP_RIGHT = 1,
	CORNER_BOTTOM_LEFT = 2,
	CORNER_BOTTOM_RIGHT = 3
} ECorner;

typedef enum {
	AXIS_X = 0,
	AXIS_Y = 1,
	AXIS_Z = 2
} EAXIS;


typedef enum {
	NEIGHBOUR_LEFT = 0,
	NEIGHBOUR_RIGHT = 1,
	NEIGHBOUR_UP = 2,
	NEIGHBOUR_DOWN = 3,
	NEIGHBOUR_FRONT = 4,
	NEIGHBOUR_BACK = 5
} NeighbourhoodPosition;

void get_chunk_pos(int px, int py, int pz, int *cx, int *cy, int *cz);
void get_pos_in_chunk(int px, int py, int pz, int *bx, int *by, int *bz,
					  int lod);
