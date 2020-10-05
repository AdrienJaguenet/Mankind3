#include "position.h"

#include <math.h>

void get_chunk_pos(int px, int py, int pz, int *cx, int *cy, int *cz)
{
	*cx = floor((float) px / CHUNK_SIZE);
	*cy = floor((float) py / CHUNK_SIZE);
	*cz = floor((float) pz / CHUNK_SIZE);
}

void get_pos_in_chunk(int px, int py, int pz, int *bx, int *by, int *bz)
{
	*bx = ((px % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE;
	*by = ((py % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE;
	*bz = ((pz % CHUNK_SIZE) + CHUNK_SIZE) % CHUNK_SIZE;
}
