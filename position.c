#include "position.h"

void get_chunk_pos(int px, int py, int pz, int *cx, int *cy, int *cz)
{
	*cx = px / CHUNK_SIZE;
	*cy = py / CHUNK_SIZE;
	*cz = pz / CHUNK_SIZE;
}

void get_pos_in_chunk(int px, int py, int pz, int *bx, int *by, int *bz)
{
	*bx = px % CHUNK_SIZE;
	*by = py % CHUNK_SIZE;
	*bz = pz % CHUNK_SIZE;
}
